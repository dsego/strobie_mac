/*
  Copyright (C) 2013 Davorin Šego
*/

using Json;
using Gee;

public class Config {

  /* Default parameters */
  public int sample_rate             = 44100;
  public int fft_sample_rate         = 44100;
  public int fft_length              = 4096;
  public int buffer_length           = 512;
  public int resampled_buffer_length = 512 * 25;
  public int periods_per_frame       = 1;
  public int estimation_framerate    = 20;
  public int strobe_framerate        = 60;
  public float audio_threshold       = -40;
  public float strobe_gain           = 1000f;
  public float pitch_standard        = 440.0f;
  public bool display_flats          = false;
  public int transpose               = 0;
  public float cents_offset          = 0f;
  public int[] partials              = { 4, 2, 1 };
  public int[] samples_per_period    = { 64, 128, 256 };
  public float[] ins_notes           = { -2900f, -2400f, -1900f , -1400f , -1000f, -500f }; /* std guitar tuning (in cents) */
  public RGB strobe_background       = RGB(0.157f, 0.110f, 0.055f);
  public RGB strobe_foreground       = RGB(0.984f, 0.627f, 0.106f);


  public Config(string filename) {
    var json_config = parse_config_file(filename);
    if (json_config != null) {
      load_from_json(json_config);
    }
  }


  private void load_from_json(Json.Object json_object) {
    if (json_object.has_member("sample_rate"))
      sample_rate = (int) json_object.get_int_member("sample_rate");

    if (json_object.has_member("fft_sample_rate"))
      fft_sample_rate = (int) json_object.get_int_member("fft_sample_rate");

    if (json_object.has_member("fft_length"))
      fft_length = (int) json_object.get_int_member("fft_length");

    if (json_object.has_member("buffer_length"))
      buffer_length = (int) json_object.get_int_member("buffer_length");

    if (json_object.has_member("resampled_buffer_length"))
      resampled_buffer_length = (int) json_object.get_int_member("resampled_buffer_length");

    if (json_object.has_member("periods_per_frame"))
      periods_per_frame = (int) json_object.get_int_member("periods_per_frame");

    if (json_object.has_member("estimation_framerate"))
      estimation_framerate = (int) json_object.get_int_member("estimation_framerate");

    if (json_object.has_member("strobe_framerate"))
      strobe_framerate = (int) json_object.get_int_member("strobe_framerate");

    if (json_object.has_member("audio_threshold"))
      audio_threshold = (float) json_object.get_double_member("audio_threshold");

    if (json_object.has_member("strobe_gain"))
      strobe_gain = (float) json_object.get_double_member("strobe_gain");

    if (json_object.has_member("pitch_standard"))
      pitch_standard = (float) json_object.get_double_member("pitch_standard");

    if (json_object.has_member("display_flats"))
      display_flats = json_object.get_boolean_member("display_flats");

    if (json_object.has_member("transpose"))
      transpose = (int) json_object.get_int_member("transpose");

    if (json_object.has_member("cents_offset"))
      cents_offset = (float) json_object.get_double_member("cents_offset");

    if (json_object.has_member("partials"))
      read_int_array(json_object.get_array_member("partials"), ref partials);

    if (json_object.has_member("samples_per_period"))
      read_int_array(json_object.get_array_member("samples_per_period"), ref samples_per_period);

    if (json_object.has_member("ins_notes     "))
      read_float_array(json_object.get_array_member("ins_notes     "), ref ins_notes     );

    if (json_object.has_member("strobe_background"))
      strobe_background = parse_from_hex(json_object.get_string_member("strobe_background"));

    if (json_object.has_member("strobe_foreground"))
      strobe_foreground = parse_from_hex(json_object.get_string_member("strobe_foreground"));
  }

  private Json.Object parse_config_file(string filename) {
    Parser parser   = new Json.Parser();
    var file_parsed = false;

    try {
      file_parsed = parser.load_from_file(filename);
    } catch (Error e) {
      stderr.printf("%s\n", e.message);
      return null;
    }

    if (file_parsed) {
      var root = parser.get_root();
      if (root == null) {
        stderr.printf("Configuration file missing root object.\n");
        return null;
      }
      return root.get_object();
    }
    return null;
  }

  private static void read_int_array(Json.Array json_array, ref int[] array) {
    var len = json_array.get_length();
    array = new int[len];
    for (var i = 0; i < len; ++i) {
      array[i] = (int) json_array.get_int_element(i);
    }
  }

  private static void read_float_array(Json.Array json_array, ref float[] array) {
    var len = json_array.get_length();
    array = new float[len];
    for (var i = 0; i < len; ++i) {
      array[i] = (float) json_array.get_double_element(i);
    }
  }

  private static RGB parse_from_hex(string hex) {
    int r = 1, g = 1, b = 1;
    if (hex.length == 4) {
      hex.down().scanf("#%1x%1x%1x", &r, &g, &b);
      r = r * 16 + r;
      g = g * 16 + g;
      b = b * 16 + b;
    }
    else if (hex.length == 7) {
      hex.down().scanf("#%2x%2x%2x", &r, &g, &b);
    }
    return RGB(r / 255f, g / 255f, b / 255f);
  }

}