/*
  Copyright (C) 2013 Davorin Šego
*/

using Json;
using Gee;

public class Config {

  /* Default parameters */
  public int sample_rate            = 44100;
  public int fft_sample_rate        = 44100;
  public int fft_length             = 4096;
  public int buffer_length          = 1024;
  public int periods_per_frame      = 1;
  public int[] partials             = { 4, 2, 1 };
  public int[] samples_per_period   = { 64, 128, 256 };

  // tunings, temperaments ?
  // strobe speed
  // audio level threshold


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

    // if (json_object.has_member("samples_per_period"))
    //   samples_per_period = (int) json_object.get_int_member("samples_per_period");

    if (json_object.has_member("periods_per_frame"))
      periods_per_frame = (int) json_object.get_int_member("periods_per_frame");

    if (json_object.has_member("partials")) {
      var array = json_object.get_array_member("partials");
      var len   = array.get_length();
      partials  = new int[len];
      for (var i = 0; i < len; ++i) {
        partials[i] = (int) array.get_int_element(i);
      }
    }
  }

  private Json.Object parse_config_file(string filename) {
    Parser parser   = new Json.Parser();
    var file_parsed = false;

    try {
      file_parsed = parser.load_from_file(filename);
    } catch (Error e) {
      stderr.printf("No configuration file. \n%s\n", e.message);
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

}