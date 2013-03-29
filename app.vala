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
  Strobe[] mono_strobes;
  Strobe[] poly_strobes;
  Display.StrobeSignal[] poly_strobe_signals;
  Display.StrobeSignal[] mono_strobe_signals;
  bool polyphonic = true;


  public App() {
    base("Strobie", 500, 400);
    config           = new Config("config.json");
    converter        = new Converter(config.buffer_length, config.fft_sample_rate, 0, 0, config.sample_rate);
    pitch_estimation = new PitchEstimation(config.fft_sample_rate, config.fft_length);

    // mono_strobes = new Strobe[config.tuning.partials.length];
    // for (var i = 0; i < count; ++i) {
    //   strobes[i] = new Strobe(config.buffer_length, config.sample_rate, config.samples_per_period);
    // }

    /* Polyphonic tuner - strobe for each note tracked */
    poly_strobes        = new Strobe[config.tuning.notes.length];
    poly_strobe_signals = new Display.StrobeSignal[config.tuning.notes.length];
    for (var i = 0; i < poly_strobes.length; ++i) {
      poly_strobes[i] = new Strobe(config.buffer_length, config.sample_rate, config.samples_per_period);
      poly_strobes[i].set_target_freq(config.tuning.notes[i].frequency);
      poly_strobe_signals[i] = { "", new float[config.samples_per_period * config.poly_periods_per_frame] };
    }




    // /* buffers for strobe outputs */
    // mono_strobe_signals = new Display.StrobeSignal[config.tuning.partials.length];

    // int len;
    // for (var i = 0; i < mono_strobe_signals.length; ++i) {
    //   len = (int) Math.floor(config.samples_per_period * config.tuning.partials[i] * config.mono_periods_per_frame);
    //   stdout.printf("%i \n", len);
    //   mono_strobe_signals[i] = { new float[len] };
    // }

    // for (var i = 0; i < poly_strobe_signals.length; ++i) {
    //   len = config.samples_per_period * config.poly_periods_per_frame;
    //   poly_strobe_signals[i] = { new float[len] };
    // }



    // void init_strobes(StrobeInit[] strobe_inits) {
    //   strobe_signals = new Display.StrobeSignal[strobe_inits.length];
    //
    //   for (var i = 0; i < strobe_inits.length; ++) {
    //     strobes[i] = new Strobe(strobe_inits[i].buffer_length, strobe_inits[i].sample_rate, strobe_inits[i].samples_per_period);
    //     strobes[i].set_target_freq(strobe_inits[i].frequency);
    //     strobe_signals[i] = { new float[strobe_inits[i].samples_per_period * strobe_inits[i].periods] };
    //   }
    // }




    // int len;
    // for (var i = 0; i < config.tuning.partials.length; ++i) {
    //   len = (int)Math.floor(config.samples_per_period * config.tuning.partials[i]);
    //   poly_strobe_signals[i] = { new float[len] };
    // }

    // strobes[i].set_target_freq(strobe_inits[i].frequency);


    // init_strobes();
    /* Polyphonic mode: create a strobe for each frequency defined in the configuration file */

    // strobe_signals = new Display.StrobeSignal[config.strobes.size];
    // strobes        = new Strobe[config.strobes.size];

    // for (var i = 0; i < config.strobes.size; ++i) {
    //   var strobe = new Strobe(config.buffer_length, config.sample_rate, config.samples_per_period);
    //   strobe.set_target_freq((float) config.strobes[i]);
    //   strobes[i]        = strobe;
    //   strobe_signals[i] = { new float[config.samples_per_period * config.periods_per_frame] };
    //   // strobe_signals.add(new float[config.samples_per_period * config.periods_per_frame]);

    // }
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
      foreach (var strobe in poly_strobes) {
        strobe.process_signal(input);
      }
    } else {

      // converter.process_signal(input);
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

    context.save();
    render_note(note.letter, note.sign, note.octave.to_string());
    context.restore();

    context.save();
    draw_strobes(mono_strobe_signals);
    // draw_signal(pitch_estimation.autocorr_data, 0.000005f);
    context.restore();

    flush();
  }

  public void draw_polyphonic() {
    context.save();
    paint_background();
    context.restore();

    context.save();
    float height = 1f / poly_strobe_signals.length;

    context.scale(this.width, this.height);
    foreach (var signal in poly_strobe_signals) {
    //   // draw_stripes(signal.data, 500f, 1f, height, 0f, top);
    //   if (signal.name != "") {
    //     ;
    //   }
      draw_stripes(signal.data, 2000f, 1f, height);
      context.translate(0, height);
    }
    context.restore();
  }


  public void do_work () {

    // converter.read(ref audio_signal);

    // var peak = find_peak(converter.output);
    // draw_level(peak);
    //
    // note = Tuning.12TET.find(110.0f);
    // draw_strobes(strobe_signals);
    if (polyphonic) {
      for (var i = 0; i < poly_strobes.length; ++i) {
        poly_strobes[i].read(ref poly_strobe_signals[i].data);
      }
      draw_polyphonic();
    } else {
      // converter.read(ref audio_signal);
      // pitch = pitch_estimation.pitch_from_autocorrelation(audio_signal);
      // note  = Tuning.12TET.find(pitch);
      // for (var i = 0; i < config.tuning.partials.length; ++i) {
      //   strobes[i].read(ref mono_strobe_signals[i].data);
      //   strobes[i].set_target_freq(pitch * config.tuning.partials[i]);
      // }
      // draw_monophonic();
    }

  }


  public static int main(string[] args) {
    if (!Thread.supported ()) {
      stderr.printf ("Cannot run without thread support.\n");
      return 1;
    }
    var app = new App();
    while (!app.quit) {
      app.do_work();
      app.process_events();
      Thread.usleep(10000);
    }
    return 0;
  }

}