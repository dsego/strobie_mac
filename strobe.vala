/*
  Copyright (C) 2013 Davorin Šego
*/

using PortAudio;

public class Strobe {

  /* strobing frequency */
  float freq;

  /* Buffers for processing data before writing to the ring buffer */
  float[] filtered_buffer;
  float[] resampled_buffer;

  /* Band pass filter */
  Biquad   bandpass;
  double[] bandpass_coeffs;

  /* Sample rate converter */
  SRC src;

  /* Circular buffer */
  Util.RingBuffer ringbuffer;
  float[] _ringbuffer;

  int sample_rate;
  int samples_per_period;


  public Strobe(int buffer_length, int sample_rate, int samples_per_period) {
    this.sample_rate        = sample_rate;
    this.samples_per_period = samples_per_period;
    bandpass                = new Biquad(3);
    src                     = new SRC(1, 1);
    filtered_buffer         = new float[buffer_length];
    resampled_buffer        = new float[buffer_length * 16];
    _ringbuffer             = new float[65536];
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
   * Set the filter band and the sample rate to a multiple of the target frequency
   */
  public void set_target_freq(float freq) {
    // stdout.printf("freq %f, new rate %f \n", freq, freq * samples_per_period);
    if (freq != this.freq) {
      this.freq = freq;
      src.set_ratio(freq * samples_per_period, sample_rate);
      src.reset();
      bandpass_coeffs = Biquad.bandpass(freq, sample_rate, 10);
    }
  }

  /**
   * Process the audio input and write into the ring buffer
   */
  public void process_signal(float[] input) {
    /* input can be smaller than the filter buffer (we don't want to read junk data) */
    weak float[] fb = filtered_buffer;
    fb.length = input.length;

    bandpass.filter(bandpass_coeffs, input, fb);                 /* IIR bandpass         */
    var count = src.linear_convert(fb, resampled_buffer);        /* re-sample            */
    ringbuffer.write(resampled_buffer, count);                   /* write to ring buffer */
  }
}