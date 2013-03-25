/*
  Copyright (C) 2013 Davorin Šego
*/

using PortAudio;

public class Converter {

  float[] filtered_buffer;
  float[] resampled_buffer;

  Biquad lowpass;
  Biquad highpass;

  double[] lowpass_coeffs;
  double[] highpass_coeffs;

  Util.RingBuffer ringbuffer;
  float[] _ringbuffer;

  /* Sample rate converter */
  SRC src;

  public Converter(int buffer_length, int fft_sample_rate, int lowpass_cutoff, int highpass_cutoff, int sample_rate) {
    src              = new SRC(fft_sample_rate, sample_rate);
    lowpass          = new Biquad(3);
    highpass         = new Biquad(3);
    lowpass_coeffs   = Biquad.lowpass(lowpass_cutoff, fft_sample_rate, 10);
    highpass_coeffs  = Biquad.highpass(highpass_cutoff, fft_sample_rate, 10);

    /* flat buffers */
    filtered_buffer  = new float[buffer_length];
    resampled_buffer = new float[buffer_length];

    /* circular buffer */
    _ringbuffer      = new float[32768];
    ringbuffer.initialize((Util.size_t) sizeof(float), _ringbuffer.length, _ringbuffer);
  }

  /**
   * Read the newest data from the ring buffer
   */
  public void read(ref float[] output) {
    while (ringbuffer.get_read_available() >= output.length) {
      ringbuffer.read(output, output.length);
    }
  }

  /**
   * Process the audio input and write into the ring buffer
   */
  public void process_signal(float[] input) {
    /* input can be smaller than the filter buffer (we don't want to read junk data) */
    weak float[] fb = filtered_buffer;
    fb.length = input.length;

    // lowpass.filter(lowpass_coeffs, input, fb); /* IIR low pass  */
    // highpass.filter(highpass_coeffs, fb, fb);  /* IIR high pass */

    // var count = src.linear_convert(input, resampled_buffer);             /* re-sample     */
    // ringbuffer.write(resampled_buffer, count);                           /* write to ring buffer */

    ringbuffer.write(input, input.length);
  }
}