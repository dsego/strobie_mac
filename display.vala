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
  protected struct RGB {
    public float r;
    public float g;
    public float b;
  }

  FontDescription font;

  Layout note_layout;
  AttrList note_attr;

  public Display(string title, int width, int height) {
    base(title, width, height);
    font = Pango.FontDescription.from_string("sans light 100");
    note_layout = Pango.cairo_create_layout(context);
    note_layout.set_font_description(font);
    note_attr = new AttrList();
    note_attr.insert(Pango.attr_scale_new(0.4));
  }



  protected void draw_strobe(float[] data) {
    // draw_stripes(data, 500f, 1f, 0.2f, 0f, 0.4f);
  }



  protected void draw_strobes(StrobeSignal[] signals) {

  }



  protected void render_note(string letter, string sign, string octave) {
    int width, height;

    context.set_source_rgba(1.0, 1.0, 1.0, 0.9);

    context.move_to(10, 10);
    note_layout.set_text(letter, -1);
    note_layout.set_alignment(Alignment.CENTER);
    note_layout.set_width(80000);
    note_layout.set_attributes(null);
    note_layout.get_pixel_size(out width, out height);
    Pango.cairo_show_layout(context, note_layout);

    note_layout.set_attributes(note_attr);
    note_layout.set_alignment(Alignment.LEFT);

    context.move_to(80, 10);
    note_layout.set_text(sign, -1);
    Pango.cairo_show_layout(context, note_layout);

    context.move_to(90, 10 + height / 2 );
    note_layout.set_text(octave, -1);
    Pango.cairo_show_layout(context, note_layout);

  }



  protected void draw_level(float level) {
    context.scale(width, height);
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



  protected void draw_wheel(float[] signal) {
    const double RADIUS_1 = 0.36;
    const double RADIUS_2 = 0.42;

    /* center */
    context.translate(width / 2, height / 2);

    /* keep aspect ratio */
    if (height > width)
      context.scale(width, width);
    else
      context.scale(height, height);


    /* Circumference divided by the number of samples to display. */
    context.set_line_width(1.25 * Math.PI * RADIUS_1 / signal.length);

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

    /* Clipping the signal */
    // var k = 4f / find_peak(output);
    var k = 4f;

    /* Draw the signal */
    foreach (float sample in signal) {
      context.set_source_rgba(1, 1, 1, k * sample);
      context.move_to(0, RADIUS_1);
      context.line_to(0, RADIUS_2);
      context.move_to(0, 0);
      context.rotate(angle);
      context.stroke();
    }
  }





  protected void draw_stripes(float[] signal, float gain = 1f, float width, float height) {
    /* Clipping the signal */
    // var peak = find_peak(signal);
    // var k = (peak > 0.01) ? 4.0 / peak : 1.0 / peak ;
    //
    // var peak = find_peak(signal);
    // stdout.printf("%f %f \n", peak, level_to_dbfs(peak));

    var gradient = new Pattern.linear(0, 0, 1, 0);
    var x        = 1.0;
    var dx       = 1.0 / (signal.length - 1);
    var count    = 0;

    /* signal -> gradient */
    foreach (float sample in signal) {
      gradient.add_color_stop_rgba(x, 1, 1, 1, gain * sample);
      // gradient.add_color_stop_rgba(x, 0.992, 0.965, 0.890, gain * sample);
      x -= dx;
      count++;
    }

    /* draw the gradient */
    context.rectangle(0, 0, width, height);
    context.set_source(gradient);
    context.fill();
  }



  protected void paint_background() {
    context.set_source_rgb(0, 0.169, 0.212);
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