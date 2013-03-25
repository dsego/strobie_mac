/*
  Copyright (C) 2013 Davorin Šego
*/

using PortAudio;
using Gee;
using Tuning;

public class App : Display {

  Config config;
  Display display;
  Converter converter;
  PitchEstimation pitch_estimation;
  Filter smoothing_filter;
  float[] smoothing_kernel;
  Stream stream;
  12TET.Note note;
  float pitch = 0.0f;

  float[] audio_signal;

  // string markup = "%s<span size='40000' rise='40000'>%s</span><span size='40000'>%i</span>\n%.1f";
  string markup = "%s<span size='40000' rise='40000'>%s</span><span size='40000'>%i</span>";


  Display.StrobeSignal[] strobe_signals;
  Strobe[] strobes;

  Strobe strobe;
  float[] strobe_signal;

  bool polyphonic = false;


  public App() {
    base(500, 400);
    config           = new Config("config.json");
    converter        = new Converter(config.buffer_length, config.fft_sample_rate, config.lowpass_cutoff, config.highpass_cutoff, config.sample_rate);
    pitch_estimation = new PitchEstimation(config.fft_sample_rate, config.fft_length);

    /* Strobe for monophonic mode */
    strobe        = new Strobe(config.buffer_length, config.sample_rate, config.samples_per_period);
    strobe_signal = new float[config.samples_per_period * config.periods_per_frame];

    /* Polyphonic mode: create a strobe for each frequency defined in the configuration file */
    strobe_signals = new Display.StrobeSignal[config.strobes.size];
    strobes        = new Strobe[config.strobes.size];

    for (var i = 0; i < config.strobes.size; ++i) {
      var strobe = new Strobe(config.buffer_length, config.sample_rate, config.samples_per_period);
      strobe.set_target_freq((float) config.strobes[i]);
      strobes[i]        = strobe;
      strobe_signals[i] = { new float[config.samples_per_period * config.periods_per_frame] };
      // strobe_signals.add(new float[config.samples_per_period * config.periods_per_frame]);
    }

    // strobe = new Strobe( config.buffer_length, config.sample_rate, config.samples_per_period);

    // strobe_signal = new float[config.samples_per_period * config.periods_per_frame];
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
    if (polyphonic) {
      foreach (var strobe in strobes) {
        strobe.process_signal(input);
      }
    } else {
      converter.process_signal(input);
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


  public void draw_monophonic() {
    context.save();
    paint_background();
    context.restore();

    // layout.set_text("%s%s %i \n%.1f".printf(note.letter, note.sign, note.octave, pitch), -1);
    context.save();
    render_text(markup.printf(note.letter, note.sign, note.octave));
    context.restore();

    context.save();
    draw_strobe(strobe_signal);
    context.restore();

    flush();
  }

  public void draw_polyphonic() {
    // context.save();
    // paint_background();
    // context.restore();

    // // context.save();
    // // draw_signal(data, 0.000005f);
    // // draw_stripes(data, 10f);
    // // context.restore();


    // // layout.set_text("%s%s %i \n%.1f".printf(note.letter, note.sign, note.octave, pitch), -1);
    // context.save();
    // render_text(markup.printf(note.letter, note.sign, note.octave));
    // context.restore();

    // flush();
  }

  public void do_work () {
    // converter.read(ref audio_signal);

    // for (var i = 0; i < strobes.length; ++i) {
    //   strobes[i].read(ref strobe_signals[i].data);
    // }

    // var peak = find_peak(converter.output);
    // draw_level(peak);
    //

    // draw_strobes(strobe_signals);


    if (polyphonic) {
      // show all
    } else {
      converter.read(ref audio_signal);
      pitch = pitch_estimation.pitch_from_autocorrelation(audio_signal);
      note = Tuning.12TET.find(pitch);
      strobe.set_target_freq((float) note.frequency);
      strobe.read(ref strobe_signal);
      draw_monophonic();
    }
  }


  public static int main(string[] args) {
    var app = new App();
    while (!app.quit) {
      app.do_work();
      app.process_events();
    }
    return 0;
  }

}