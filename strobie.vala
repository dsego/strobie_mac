/*
  Copyright (C) 2013 Davorin Šego
*/

using PortAudio;

public class Strobie : Display {

  Config config;

  Biquad   bandpass_filter;
  double[] bandpass_coeffs;
  float[]  bandpassed_data;

  PitchEstimation pitch_estimation;

  Filter fir;
  float[] smoothing_kernel;

  Stream stream;
  SRC src;

  /* Resampled data */
  Util.RingBuffer src_rb;
  float[] _src_rb;
  float[] src_data;

  /* Raw audio data */
  Util.RingBuffer raw_rb;
  float[] _raw_rb;
  float[] raw_data;

  float[] output;
  float pitch = 0.0f;






  public Strobie() {
    /* create the window (call this method first or it will segfault) */
    base(500, 400);

    /* load configuration */
    config = new Config("");


    /* sample rate converter, filter, pitch estimator */
    src              = new SRC(1, 1);
    bandpass_filter  = new Biquad(3);
    pitch_estimation = new PitchEstimation(config.sample_rate, config.fft_length);

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
    raw_data         = new float[config.raw_buffer_length];
    bandpassed_data  = new float[config.src_buffer_length];
    src_data         = new float[config.src_buffer_length * 4];
    output           = new float[config.samples_per_period * config.periods_per_frame];

    /* circular buffers */
    _src_rb = new float[32768];
    _raw_rb = new float[32768];
    src_rb.initialize((Util.size_t) sizeof(float), _src_rb.length, _src_rb);
    raw_rb.initialize((Util.size_t) sizeof(float), _raw_rb.length, _raw_rb);

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
    while (src_rb.get_read_available() >= output.length) {
      src_rb.read(output, output.length);
    }
  }

  /**
   * Fetch audio data from the sound card and process
   */
  private int stream_callback(void* input, void* output, ulong nframes, Stream.CallbackTimeInfo time, Stream.CallbackFlags flags) {
    unowned float[] finput;

    raw_rb.write((float[]) input, (int) nframes);

    int count = (int) nframes;
    int index = 0;

    /* process in batches because nframes is variable */
    while (count > config.src_buffer_length) {
      finput = (float[]) (&input[index]);
      finput.length = (int) config.src_buffer_length;
      process_signal(finput);
      count -= config.src_buffer_length;
      index += config.src_buffer_length;
    }

    /* anything left over */
    if (count > 0) {
      finput = (float[]) (&input[index]);
      finput.length = count;
      process_signal(finput);
    }

    return 0;
  }

  private void process_signal(float[] input) {
    // unowned float[] bp_data = bandpassed_data;
    // bp_data.length = input.length;

    /* IIR band pass */
    bandpass_filter.filter(bandpass_coeffs, input, bandpassed_data);

    /* convert sample rate  */
    // var count = src.linear_convert(bp_data, src_data);

    /* write to ring buffer */
    // src_rb.write(src_data, count);
  }













  public float find_pitch() {
    if (raw_rb.get_read_available() <= raw_data.length)
      return pitch;

    while (raw_rb.get_read_available() >= raw_data.length) {
      raw_rb.read(raw_data, raw_data.length);
    }

    // peak = fir.process(pitch_estimation.pitch_from_autocorrelation(raw_data));
    pitch = pitch_estimation.pitch_from_autocorrelation(raw_data);
    return pitch;
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
    var pitch   = 0.0f;

    while (!strobie.quit) {
      // strobie.set_target_freq(329.628f);
      strobie.read_output(); /* will segfault if this line is missing - why ? */

      pitch = strobie.find_pitch();

      // strobie.draw_signal(strobie.output, 500f);
      strobie.render_text(pitch);
      // display.draw_stripes(strobie.output, 500f);
      strobie.process_events();
    }

    return 0;
  }

}