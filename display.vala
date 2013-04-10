/*
  Copyright (C) 2013 Davorin Šego
*/

using Cairo;
using Pango;

public class Display : GLCairoWindow {

  protected struct StrobeSignal {
    public string name;
    public float[] data;
  }

  // Layout note_letter_layout;
  // Layout note_octave_layout;
  // Layout note_sign_layout;
  // FontDescription font;
  // AttrList note_attr;

  public Display(string title, int width, int height) {
    base(title, width, height);
    set_font();
  }

  protected void set_font() {
    Cairo.FontFace font_face;
    try {
      font_face = FontLoader.load("fonts/DejaVuSansMono.ttf");
    } catch (Error e) {
      stderr.puts("Error! Font file is missing.\n");
      return;
    }

    // context.select_font_face("Monaco", FontSlant.NORMAL, FontWeight.NORMAL);
    context.set_font_face(font_face);
  }

  protected void init_text_layouts() {
    // font      = Pango.FontDescription.from_string("sans light 100");
    // note_attr = new AttrList();
    // note_attr.insert(Pango.attr_scale_new(0.4));

    // note_letter_layout = Pango.cairo_create_layout(context);
    // note_octave_layout = Pango.cairo_create_layout(context);
    // note_sign_layout   = Pango.cairo_create_layout(context);

    // note_letter_layout.set_font_description(font);
    // note_octave_layout.set_font_description(font);
    // note_sign_layout.set_font_description(font);

    // note_sign_layout.set_attributes(note_attr);
    // note_octave_layout.set_attributes(note_attr);
  }


  protected void draw_strobe(float[] data) {
    // draw_stripes(data, 500f, 1f, 0.2f, 0f, 0.4f);
  }



  protected void draw_strobes(StrobeSignal[] signals) {

  }



  protected void render_note(string letter, string sign, string octave) {
    // int w1, h1, w2, h2;

    TextExtents extents1;
    TextExtents extents2;

    context.set_source_rgba(1.0, 1.0, 1.0, 0.9);
    context.set_font_size(96);
    context.text_extents(letter, out extents1);

    context.set_font_size(32);
    context.text_extents(octave, out extents2);

    context.move_to((window_width - extents1.width - extents2.width) / 2, 100);
    context.set_font_size(96);
    context.show_text(letter);

    context.set_font_size(32);
    context.show_text(octave);
    context.rel_move_to(-extents2.width, -extents2.height);
    context.show_text(sign);
    // context.rel_move_to(10, 48);





    // note_letter_layout.context_changed();
    // note_letter_layout.set_text(letter, 1);
    // note_letter_layout.get_pixel_size(out w1, out h1);

    // note_sign_layout.set_text(sign, -1);
    // note_octave_layout.set_text(octave, -1);
    // note_octave_layout.get_pixel_size(out w2, out h2);

    // context.move_to((window_width - (w1 + w2 + 10)) / 2, 50);
    // Pango.cairo_show_layout(context, note_letter_layout);

    // context.rel_move_to(70, 0);
    // Pango.cairo_show_layout(context, note_sign_layout);

    // context.rel_move_to(10, h1 / 2);
    // Pango.cairo_show_layout(context, note_octave_layout);
  }



  protected void draw_level(float level) {
    context.scale(window_width, window_height);
    context.translate(0, 0.5);
    context.rectangle(0, -0.1, level, 0.2);
    // context.set_source(gradient);
    context.set_source_rgb(0.4, 0.8, 0.7);
    context.fill();
  }


  protected void draw_signal(float[] signal, float gain = 1f) {
    float dx = (float) 1f / (signal.length - 1);
    float x  = 0f;
    context.set_line_width(0.001);
    context.set_source_rgb(1, 1, 1);
    context.move_to(x, -signal[0] * gain);
    for (int i = 1; i < signal.length; ++i) {
      x += dx;
      context.line_to(x, -signal[i] * gain);
    }
    context.stroke();
  }



  protected void draw_wheel(float[] signal, float gain = 1f) {
    const double RADIUS_1 = 0.36;
    const double RADIUS_2 = 0.42;

    /* Circumference divided by the number of samples to display. */
    context.set_line_width(0.2 * Math.PI * RADIUS_1 / signal.length);
    context.set_line_cap(Cairo.LineCap.SQUARE);

    /* Negative angle - rotates counter-clockwise if the note is flat, clockwise if it is sharp */
    var angle = -Math.PI * 2 / signal.length;

    /* Draw the dimmed circle. */
    foreach (float sample in signal) {
      context.set_source_rgba(0.6, 0.8, 1, 0.1);
      context.move_to(0, RADIUS_1);
      context.line_to(0, RADIUS_2);
      context.move_to(0, 0);
      context.rotate(angle);
      context.stroke();
    }

    /* Draw the signal */
    foreach (float sample in signal) {
      context.set_source_rgba(1, 1, 1, gain * sample);
      context.move_to(0, RADIUS_1);
      context.line_to(0, RADIUS_2);
      context.move_to(0, 0);
      context.rotate(angle);
      context.stroke();
    }
  }


  protected void draw_stripes(float[] signal, float gain = 1f, float width, float height, RGB fg) {
    var gradient = new Pattern.linear(0, 0, width, 0);
    gradient.set_filter(Cairo.Filter.FAST);
    var x        = 1f;
    var dx       = 1f / (signal.length - 1);
    var count    = 0;

    /* signal -> gradient */
    foreach (float sample in signal) {
      gradient.add_color_stop_rgba(x, fg.r, fg.g, fg.b, gain * sample);
      x -= dx;
      count++;
    }

    /* draw the gradient */
    context.rectangle(0, 0, width, height);
    context.set_source(gradient);
    context.fill();
  }

  protected void strobe_display(StrobeSignal[] strobe_signals, float gain, RGB bg, RGB fg) {
    var len = strobe_signals.length;
    context.set_source_rgb(bg.r, bg.g, bg.b);
    context.rectangle(40, 200, 420, 81 * len);
    context.fill();
    context.save();
    context.translate(40, 200);
    foreach (var signal in strobe_signals) {
      draw_stripes(signal.data, gain, 420, 80, fg);
      context.translate(0, 81);
    }
    context.restore();
  }

  protected void window_background() {
    // context.set_source_rgb(0, 0.169, 0.212);
    // context.set_source_rgb(0.1, 0.02, 0.0);
    context.set_source_rgb(0.0, 0.0, 0.0);
    context.paint();
  }

  private void normalize(float[] data) {
    var max = 0f;

    /* find absolute max */
    for (int i = 1; i < data.length; ++i) {
      if (data[i] > max)
        max = data[i];
      else if (data[i] < -max)
        max = -data[i];
    }
    var k = 1f / max;

    for (int i = 1; i < data.length; ++i) {
      data[i] *= k;
    }
  }

}