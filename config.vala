/*
  Copyright (C) 2013 Davorin Šego
*/

using Json;
using Gee;

public class Config {

  public int sample_rate           = 44100;
  public int fft_sample_rate       = 44100;
  // public int fft_sample_rate       = 4000;
  public int fft_length            = 4096;
  public int buffer_length         = 1024;
  public int samples_per_period    = 512;
  public int periods_per_frame     = 2;
  public int lowpass_cutoff        = 2000;
  public int highpass_cutoff       = 60;
  public ArrayList<double?> strobes = new ArrayList<double?>();


  public Config(string filename) {

    strobes.add(329.628); /* E4 - high E */
    strobes.add(246.942); /* B3 */
    strobes.add(195.998); /* G3 */
    strobes.add(146.832); /* D3 */
    strobes.add(110.000); /* A2 */
    strobes.add(82.4069); /* E2 - low E */

    Parser parser   = new Json.Parser();
    var file_parsed = false;

    try {
      file_parsed = parser.load_from_file(filename);
    } catch (Error e) {
      stderr.printf("No configuration file. \n%s\n", e.message);
    }

    if (file_parsed) {
      var root     = parser.get_root();
      var root_obj = root.get_object();

      if (root_obj.has_member("sample_rate"))
        sample_rate = (int) root_obj.get_int_member("sample_rate");

      if (root_obj.has_member("fft_sample_rate"))
        fft_sample_rate = (int) root_obj.get_int_member("fft_sample_rate");

      if (root_obj.has_member("fft_length"))
        fft_length = (int) root_obj.get_int_member("fft_length");

      if (root_obj.has_member("buffer_length"))
        buffer_length = (int) root_obj.get_int_member("buffer_length");

      if (root_obj.has_member("samples_per_period"))
        samples_per_period = (int) root_obj.get_int_member("samples_per_period");

      if (root_obj.has_member("periods_per_frame"))
        periods_per_frame  = (int) root_obj.get_int_member("periods_per_frame");

      if (root_obj.has_member("lowpass_cutoff"))
        lowpass_cutoff  = (int) root_obj.get_int_member("lowpass_cutoff");

      if (root_obj.has_member("highpass_cutoff"))
        highpass_cutoff  = (int) root_obj.get_int_member("highpass_cutoff");

      if (root_obj.has_member("strobes")) { stdout.puts("yes\n");
        var json_strobes = root_obj.get_array_member("strobes");
        var length       = json_strobes.get_length();
        if (length > 0) {
          strobes = new ArrayList<double?>();
          for (var i = 0; i < length; ++i) {
            var el =  json_strobes.get_double_element(i);
            if (el != 0) strobes.add(el);
          }
        }
      }
    }
  }

}