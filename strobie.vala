/*
  Copyright (C) 2013 Davorin Šego
*/

using PortAudio;

public class Strobie {

  Config config;

  Biquad bandpass_filter;
  double[] bandpass_coeffs;
  float[] bandpassed_data;

  PitchEstimation pitch_estimation;

  Filter fir;
  float[] smoothing_kernel;

  Stream stream;
  SRC src;

  Util.RingBuffer ringbuffer2;
  Util.RingBuffer rinbuffer1;
  float[] _ringbuffer2;
  float[] _rinbuffer1;

  float peak;
  float[] raw_data;
  float[] resampled_data;
  float[] output;






  public Strobie() {

    /* load configuration */
    config = new Config("");

    /* sample rate converter, filters, pitch estimator */
    src              = new SRC(1, 1);
    bandpass_filter  = new Biquad(3);
    pitch_estimation = new PitchEstimation(config.sample_rate, 1024);

    /* Smoothing filter */
    // float[] coeffs = { 1f, 1f, 1f, 1f, 1f };
    // float[] coeffs = { 0.25f, 0.5f, 0.25f };
    // Window.gaussian(coeffs);
    // smoothing_kernel = gaussian(11);
    // fir = new Filter(smoothing_kernel);

    // var gain = 0f;
    // foreach (float x in smoothing_kernel) {
    //   gain += x;
    // }

    // stdout.printf("%f \n", gain);

    /* flat buffers */
    raw_data         = new float[1024];
    bandpassed_data  = new float[config.buffer_length];
    resampled_data   = new float[config.buffer_length * 4];
    output           = new float[config.samples_per_period * config.periods_per_frame];

    /* circular buffers */
    _ringbuffer2 = new float[32768];
    _rinbuffer1  = new float[32768];
    ringbuffer2.initialize((Util.size_t) sizeof(float), _ringbuffer2.length, _ringbuffer2);
    rinbuffer1.initialize((Util.size_t) sizeof(float), _rinbuffer1.length, _rinbuffer1);

    /* start PortAudio and open the input stream*/
    PortAudio.initialize();
    Stream.open_default(out stream, 1, 0, FLOAT_32, config.sample_rate, FRAMES_PER_BUFFER_UNSCPECIFIED, stream_callback);
    stream.start();
  }

  ~Strobie() {
    stream.stop();
  }






  /**
   * Read the newest data from the ring buffer into the output buffer
   */
  public void read_output() {
    while (rinbuffer1.get_read_available() >= output.length) {
      rinbuffer1.read(output, output.length);
    }
  }

  /**
   * Fetch audio data from the sound card and process
   */
  private int stream_callback(void* input, void* output, ulong nframes, Stream.CallbackTimeInfo time, Stream.CallbackFlags flags) {
    unowned float[] finput;
    int i = 0;

    while (nframes > config.buffer_length) {
      finput = (float[]) (&input[i]);
      finput.length = config.buffer_length;
      process_signal(finput);
      nframes -= config.buffer_length;
      i += config.buffer_length;
    }

    if (nframes > 0) {
      finput = (float[]) (&input[i]);
      finput.length = (int) nframes;
      process_signal(finput);
    }

    return 0;
  }

  private void process_signal(float[] input) {
    ringbuffer2.write(input, input.length);

    unowned float[] data = bandpassed_data;
    data.length = input.length;
    bandpass_filter.filter(bandpass_coeffs, input, data);

    /* Convert sample rate */
    var count = src.linear_convert(data, resampled_data);

    /* Write to ring buffer */
    rinbuffer1.write(resampled_data, count);
  }













  public void find_pitch() {
    if (ringbuffer2.get_read_available() <= raw_data.length)
      return;

    while (ringbuffer2.get_read_available() >= raw_data.length) {
      ringbuffer2.read(raw_data, raw_data.length);
    }

    peak = fir.process(pitch_estimation.pitch_from_autocorrelation(raw_data));
    // peak = pitch_estimation.pitch_from_autocorrelation(raw_data);
  }

  private float find_peak(float[] buffer) {
    var peak = 0f;
    foreach (float sample in buffer) {
      if (sample < -peak)
        peak = -sample;
      else if (sample > peak)
        peak = sample;
    }
    return peak;
  }

  public void set_target_freq(float freq) {
    src.set_ratio(freq * config.samples_per_period, config.sample_rate);
    src.reset();
    bandpass_coeffs = Biquad.bandpass(freq, config.sample_rate, 10);
  }

  public static double level_to_dbfs(double level) {
    return 20.0 * Math.log10(level);
  }

  public static double dbfs_to_level(double dbfs) {
    return Math.pow(10.0, dbfs / 20.0);
  }








  public static int main(string[] args) {

    var strobie = new Strobie();
    var display = new Display(500, 400);

    strobie.set_target_freq(329.628f);

    while (!display.quit) {
      strobie.read_output();
      display.draw_stripes(strobie.output, 500f);
      display.process_events();
    }

    return 0;
  }

}