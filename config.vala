/*
  Copyright (C) 2013 Davorin Šego
*/

using Json;
using Gee;

public class Config {

  public struct Note {
    public string name;
    public float frequency;
    public Note (string name, float frequency) {
      this.name      = name;
      this.frequency = frequency;
    }
  }

  public struct Tuning {
    public string description;
    public string scale;
    public float concert_a;
    public float cent_offset;
    public float[]? partials;
    public Note[]? notes;
    public Tuning (string d, string s, float ca, float co, float[] part, Note[] notes) {
      this.description = d;
      this.scale       = s;
      this.concert_a   = ca;
      this.cent_offset = ca;
      this.cent_offset = co;
      this.partials    = part;
      this.notes       = notes;
    }
  }



  /* Default parameters */

  public int sample_rate            = 44100;
  public int fft_sample_rate        = 44100;
  public int fft_length             = 4096;
  public int buffer_length          = 1024;
  public int samples_per_period     = 128;
  public int mono_periods_per_frame = 1;
  public int poly_periods_per_frame = 5;
  public string tuning_name         = null;

  // public int lowpass_cutoff         = 2000;
  // public int highpass_cutoff        = 60;

  public Tuning tuning = Tuning(
    "Standard guitar tuning", "12TET", 440.0f,  0.0f,
    { 1, 2, 4, 8 },
    { Note("e'", 329.628f), Note("b", 246.942f), Note("g", 195.998f),
      Note("d",  146.832f), Note("A", 110.000f), Note("E", 82.4069f)}
  );

  // strobe speed ?
  // audio level threshold


  public Config(string filename) {
    var json_config = parse_config_file(filename);
    if (json_config != null) {
      load_from_json(json_config);
    }
    if (tuning_name != null) {
      // load tuning file
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

    if (json_object.has_member("samples_per_period"))
      samples_per_period = (int) json_object.get_int_member("samples_per_period");

    if (json_object.has_member("mono_periods_per_frame"))
      mono_periods_per_frame = (int) json_object.get_int_member("mono_periods_per_frame");

    if (json_object.has_member("poly_periods_per_frame"))
      poly_periods_per_frame  = (int) json_object.get_int_member("poly_periods_per_frame");

    if (json_object.has_member("tuning_name"))
      tuning_name  = json_object.get_string_member("tuning_name");

    // if (root_obj.has_member("lowpass_cutoff"))
    //   lowpass_cutoff  = (int) root_obj.get_int_member("lowpass_cutoff");

    // if (root_obj.has_member("highpass_cutoff"))
    //   highpass_cutoff  = (int) root_obj.get_int_member("highpass_cutoff");
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