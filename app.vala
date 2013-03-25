/*
  Copyright (C) 2013 Davorin Šego
*/

using PortAudio;

public class App : Display {

  Config config;
  Strobe strobe;
  Display display;
  Converter converter;
  PitchEstimation pitch_estimation;
  Filter smoothing_filter;
  float[] smoothing_kernel;
  Stream stream;
  float pitch = 0.0f;

  float[] strobe_signal;
  float[] audio_signal;


  public App() {
    base(500, 400);
    config           = new Config("config.json");
    converter        = new Converter(config.buffer_length, config.fft_sample_rate, config.lowpass_cutoff, config.highpass_cutoff, config.sample_rate);
    strobe           = new Strobe( config.buffer_length, config.sample_rate, config.samples_per_period);
    pitch_estimation = new PitchEstimation(config.fft_sample_rate, config.fft_length);

    strobe_signal = new float[config.samples_per_period * config.periods_per_frame];
    audio_signal  = new float[config.fft_length];
    // audio_signal  = new float[128];

    /* start PortAudio and open the input stream*/
    PortAudio.initialize();
    Stream.open_default(out stream, 1, 0, FLOAT_32, config.sample_rate, FRAMES_PER_BUFFER_UNSCPECIFIED, stream_callback);
    stream.start();
  }

  ~App() {
    stream.stop();
  }

  /**
   * Fetch audio data from the sound card and process
   */
  int stream_callback(void* input, void* output, ulong nframes, Stream.CallbackTimeInfo time, Stream.CallbackFlags flags) {
    unowned float[] finput;
    int count = (int) nframes;
    int index = 0;

    /* process in batches because nframes can be bigger than the allocated buffer size */
    while (count > config.buffer_length) {
      finput = (float[]) (&input[index]);
      finput.length = config.buffer_length;
      process_signal(finput);
      count -= config.buffer_length;
      index += config.buffer_length;
    }

    /* anything left over */
    if (count > 0) {
      finput = (float[]) (&input[index]);
      finput.length = count;
      process_signal(finput);
    }

    return 0;
  }

  void process_signal(float[] input) {
    converter.process_signal(input);
    strobe.process_signal(input);
  }

  float find_peak(float[] buffer) {
    var peak = 0f;
    foreach (float sample in buffer) {
      if (sample < -peak)
        peak = -sample;
      else if (sample > peak)
        peak = sample;
    }
    return peak;
  }

  double to_dbfs(double value) {
    return 20.0 * Math.log10(value);
  }



    /* smooth the level measurements */
    // smoothing_kernel = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    // Window.gaussian(smoothing_kernel);
    // smoothing_filter = new Filter(smoothing_kernel);

      // app.set_target_freq(329.628f);
      // app.read_strobe_signal();

      // app.draw_signal(app.strobe_signal, 500f);

      // app.draw_stripes(app.strobe_signal, 500f);

      // app.draw_level(app.level);

  public void init() {
    strobe.set_target_freq(329.628f);
  }

  public void do_work () {
    // converter.read(ref audio_signal);
    strobe.read(ref strobe_signal);

    // var peak = find_peak(converter.output);
    // draw_level(peak);

    draw_strobe(strobe_signal);

    // pitch = pitch_estimation.pitch_from_autocorrelation(audio_signal);
    // pitch = pitch_estimation.pitch_from_fft(audio_signal);

    // draw(pitch_estimation.spectrum, pitch);
    // draw(pitch_estimation.autocorrelation, pitch);
  }


  public static int main(string[] args) {
    var app = new App();
    app.init();
    while (!app.quit) {
      app.do_work();
      app.process_events();
    }
    return 0;
  }

}