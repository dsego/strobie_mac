/*
  Copyright (C) 2013 Davorin Šego
*/

using PortAudio;
using Gee;
using Tuning;

public class App : Display {

  Config config;
  Converter converter;
  PitchEstimation pitch_estimation;
  Stream stream;
  12TET.Note note;
  float pitch   = 0.0f;
  float[] audio_signal;
  Strobe[] strobes;
  Display.StrobeSignal[] strobe_signals;

  public App() {
    base("Strobie", 500, 400);

    config           = new Config("config.json");
    converter        = new Converter(config.buffer_length, config.fft_sample_rate, 0, 0, config.sample_rate);
    pitch_estimation = new PitchEstimation(config.fft_sample_rate, config.fft_length);
    strobes          = new Strobe[config.partials.length];
    strobe_signals   = new Display.StrobeSignal[strobes.length];

    var samples_per_period = config.samples_per_period;
    for (var i = 0; i < strobes.length; ++i) {
      strobes[i] = new Strobe(config.buffer_length, config.sample_rate, config.samples_per_period[i]);
      strobe_signals[i] = { "", new float[config.samples_per_period[i] * config.periods_per_frame * config.partials[i]] };
    }

    audio_signal  = new float[config.fft_length];

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
    foreach (var strobe in strobes) {
      strobe.process_signal(input);
    }
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


  public void draw() {return;
    paint_background();

    context.save();
    note = Tuning.12TET.find(pitch);
    render_note(note.letter, note.sign, note.octave.to_string());
    context.restore();

    float height = 0.7f / strobe_signals.length;
    context.save();
    context.scale(this.width, this.height);
    context.translate(0, 0.3);
    foreach (var signal in strobe_signals) {
    //   // draw_stripes(signal.data, 500f, 1f, height, 0f, top);
    //   if (signal.name != "") {
    //     ;
    //   }
      draw_stripes(signal.data, 2000f, 1f, height);
      context.translate(0, height);
    }
    // draw_signal(pitch_estimation.autocorr_data, 0.000005f);
    context.restore();


    // context.save();
    // /* center */
    // context.translate(width / 2, height / 2);

    // /* keep aspect ratio */
    // if (height > width)
    //   context.scale(width, width);
    // else
    //   context.scale(height, height);

    // draw_wheel(strobe_signals[2].data, 2000f);
    // context.restore();
  }


  public void strobe() {
    for (var i = 0; i < strobes.length; ++i) {
      strobes[i].read(ref strobe_signals[i].data);
      strobes[i].set_target_freq((float) note.frequency * config.partials[i]);
    }
    draw();
  }

  public int find_pitch() {
    while (true) {
      converter.read(ref audio_signal);
      var p = pitch_estimation.pitch_from_autocorrelation(audio_signal);
      pitch = p.clamp(27.5000f, 4186.01f); /* A0 - C8 */
      Thread.usleep(20000);
    }
    return 0;
  }


  public static int main(string[] args) {
    if (!Thread.supported ()) {
      stderr.printf ("Cannot run without thread support.\n");
      return 1;
    }
    var app = new App();
    Thread<int> thread = new Thread<int>("", app.find_pitch);
    while (!app.quit) {
      app.strobe();
      app.process_events();
      Thread.usleep(10000);
    }
    return 0;
  }

}