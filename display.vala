/*
  Copyright (C) 2013 Davorin Šego
*/

using Cairo;
using Pango;

public class Display : SDLCairoWindow {

  string text = "";
  FontDescription font;
  ImageSurface background;
  int bg_width;
  int bg_height;


  public Display(int width, int height) {
    base(width, height);
    load_background("pattern.png");
    load_font();
  }




  public void load_background(string filename) {
    background = new ImageSurface.from_png(filename);
    bg_width   = background.get_width();
    bg_height  = background.get_height();
  }

  public void load_font() {
    font = new Pango.FontDescription();
    font.set_size(100000);
    font.set_family("sans");
    font.set_style(Pango.Style.NORMAL);
  }

  public void paint_background() {
    context.scale((double )screen.w / bg_width, (double) screen.h / bg_height);
    context.set_source_surface(background, 0, 0);
    context.paint();
  }

  public void render_text(float pitch) {
    var layout = Pango.cairo_create_layout(context);
    layout.set_font_description(font);
    // layout.set_text("%.1f".printf(pitch), -1);

    var note = Tuning.12TET.find(pitch);
    // layout.set_text("%s%s %i \n%.1f".printf(note.letter, note.sign, note.octave, pitch), -1);
    layout.set_markup("%s<sub>%s%i</sub>\n%.1f".printf(note.letter, note.sign, note.octave, pitch), -1);

    // int width, height;
    // layout.get_size(out width, out height);

    context.set_source_rgb(0.0, 0.0, 0.0);
    context.paint();

    context.move_to(0, 0);
    context.set_source_rgba(1.0, 1.0, 1.0, 0.9);
    Pango.cairo_update_layout(context, layout);
    Pango.cairo_show_layout(context, layout);

    CairoSDL.surface_flush(cairo_surface);
    surface.blit(null, screen, null);
    screen.flip();
  }







  public void draw_signal(float[] signal, float gain = 1f) {
    float x  = 0;
    float y  = surface.h / 2;
    float dx = (float) surface.w / (signal.length - 1);

    context.set_source_rgb(0.0, 0.0, 0.0);
    context.paint();
    context.set_line_width(1);
    context.set_source_rgb(0.4, 0.8, 0.7);

    context.move_to(x, y - signal[0] * gain);
    for (int i = 1; i < signal.length; ++i) {
      x += dx;
      context.line_to(x, y - signal[i] * gain);
    }
    context.stroke();

    CairoSDL.surface_flush(cairo_surface);
    surface.blit(null, screen, null);
    screen.flip();
  }



  public void draw_wheel(float[] signal) {

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

    context.restore();

    CairoSDL.surface_flush(cairo_surface);
    surface.blit(null, screen, null);
    screen.flip();
  }





  public void draw_stripes(float[] signal, float gain = 1f) {
    context.save();
    paint_background();
    context.restore();

    /* Clipping the signal */
    // var peak = find_peak(signal);
    // var k = (peak > 0.01) ? 4.0 / peak : 1.0 / peak ;
    //
    // var peak = find_peak(signal);
    // stdout.printf("%f %f \n", peak, level_to_dbfs(peak));

    var gradient = new Pattern.linear(0, 0, 1, 0);

    var x     = 1.0;
    var dx    = 1.0 / (signal.length - 1);
    var count = 0;

    /* signal -> gradient */
    foreach (float sample in signal) {
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

}