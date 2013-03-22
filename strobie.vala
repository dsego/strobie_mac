/*
  Copyright (C) 2012 Davorin Šego
*/

using PortAudio;
using Cairo;
using Pango;

public class Strobie : SDLCairoWindow {

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

  string text = "";
  FontDescription font;
  ImageSurface background;
  int bg_width;
  int bg_height;

  public Strobie() {

    /* create the display (call this method first or it will segfault) */
    base(500, 400);

    /* load configuration */
    config = new Config("");

    /* load background */
    background = new ImageSurface.from_png("pattern.png");
    bg_width   = background.get_width();
    bg_height  = background.get_height();

    /* load font */
    font = new Pango.FontDescription();
    font.set_size(100000);
    font.set_family("sans");
    font.set_style(Pango.Style.NORMAL);

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

  /**
   * Read the newest data from the ring buffer into the output buffer
   */
  public void read_output() {
    while (rinbuffer1.get_read_available() >= output.length) {
      rinbuffer1.read(output, output.length);
    }
  }

  private void paint_background() {
    context.scale((double )screen.w / bg_width, (double) screen.h / bg_height);
    context.set_source_surface(background, 0, 0);
    context.paint();
  }

  private  void render_text() {
    var layout = Pango.cairo_create_layout(context);
    layout.set_font_description(font);
    layout.set_text("%.1f".printf(peak), -1);

    // var note = Tuning.12TET.find(peak);
    // layout.set_text("%s%s %i".printf(note.letter, note.sign, note.octave), -1);

    // int width, height;
    // layout.get_size(out width, out height);
    context.move_to(0, 0);

    context.set_source_rgba(1.0, 1.0, 1.0, 0.9);
    Pango.cairo_update_layout(context, layout);
    Pango.cairo_show_layout(context, layout);
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

  public void draw_signal(float[] data, float gain = 1f) {
    float x  = 0;
    float y  = surface.h / 2;
    float dx = (float) surface.w / (data.length - 1);

    this.context.set_source_rgb(0.0, 0.0, 0.0);
    this.context.paint();
    this.context.set_line_width(1);
    this.context.set_source_rgb(0.4, 0.8, 0.7);

    this.context.move_to(x, y - data[0] * gain);
    for (int i = 1; i < data.length; ++i) {
      x += dx;
      context.line_to(x, y - data[i] * gain);
    }
    this.context.stroke();

    render_text();

    CairoSDL.surface_flush(cairo_surface);
    surface.blit(null, screen, null);
    screen.flip();
  }

  /**
   * Draw the strobe wheel
   */
  public void draw_wheel() {

    const double RADIUS_1 = 0.36;
    const double RADIUS_2 = 0.42;

    context.save();
    paint_background();
    context.restore();

    context.save();

    /* center */
    context.translate(surface.w / 2, surface.h / 2);

    /* keep aspect ratio */
    if (surface.h > surface.w)
      context.scale(surface.w, surface.w);
    else
      context.scale(surface.h, surface.h);


    /* Circumference divided by the number of samples to display. */
    context.set_line_width(1.25 * Math.PI * RADIUS_1 / output.length);

    context.set_line_cap(Cairo.LineCap.SQUARE);


    /* Negative angle - rotates counter-clockwise if the note is flat, clockwise if it is sharp */
    var angle = -Math.PI * 2 / output.length;

    /* Draw the dimmed circle. */
    foreach (float sample in output) {
      context.set_source_rgba(0.6, 0.8, 1, 0.1);
      context.move_to(0, RADIUS_1);
      context.line_to(0, RADIUS_2);
      context.move_to(0, 0);
      context.rotate(angle);
      context.stroke();
    }

    /* Clipping the signal */
    // var k = 4f / find_peak(output);
    var k = 4f;

    /* Draw the signal */
    foreach (float sample in output) {
      context.set_source_rgba(1, 1, 1, k * sample);
      context.move_to(0, RADIUS_1);
      context.line_to(0, RADIUS_2);
      context.move_to(0, 0);
      context.rotate(angle);
      context.stroke();
    }

    context.restore();

    CairoSDL.surface_flush(cairo_surface);
    surface.blit(null, screen, null);
    screen.flip();
  }

  private void draw_stripes(float gain = 1f) {
    context.save();
    paint_background();
    context.restore();

    /* Clipping the signal */
    // var peak = find_peak(output);
    // var k = (peak > 0.01) ? 4.0 / peak : 1.0 / peak ;
    //
    // var peak = find_peak(output);
    // stdout.printf("%f %f \n", peak, level_to_dbfs(peak));

    var gradient = new Pattern.linear(0, 0, 1, 0);

    var x     = 1.0;
    var dx    = 1.0 / (output.length - 1);
    var count = 0;

    /* signal -> gradient */
    foreach (float sample in output) {
      gradient.add_color_stop_rgba(x, 1, 1, 1, gain * sample);
      x -= dx;
      count++;
    }

    /* draw the gradient */
    context.save();
    context.scale(surface.w, surface.h);
    context.translate(0, 0.5);
    context.rectangle(0, -0.1, 1, 0.2);
    context.set_source(gradient);
    context.fill();
    context.restore();

    CairoSDL.surface_flush(cairo_surface);
    surface.blit(null, screen, null);
    screen.flip();
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

    // strobie.set_target_freq(220);
    // strobie.set_target_freq(146.832f);
    strobie.set_target_freq(329.628f);


    // strobie.set_target_freq(659.255f); // E4
    // strobie.set_target_freq(493.883f); // B4
    // strobie.set_target_freq(391.995f); // G4
    // strobie.set_target_freq(293.665f); // D4
    // strobie.set_target_freq(440.000f); // A4
    // strobie.set_target_freq(329.628f); // E4

    while (!strobie.quit) {
      // strobie.find_pitch();
      strobie.read_output();
      // strobie.draw_signal(strobie.lowpassed_data, 1000);
      // strobie.draw_signal(strobie.pitch_estimation.autocorrelation, 0.1f);
      // strobie.draw_signal(strobie.smoothing_kernel, 100f);
      // strobie.draw_signal(strobie.pitch_estimation.padded_data, 1000f);
      strobie.draw_stripes(500f);
      strobie.process_events();
    }
    return 0;
  }

}