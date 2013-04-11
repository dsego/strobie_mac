/**
 *  IIR Biquad filter
 *
 *  Biquad formulas from:
 *    Nigel Redmon
 *    http://www.earlevel.com/main/2011/01/02/biquad-formulas/
 */
class Biquad {

  public Biquad(int sectionCount) {
    this.z1 = new double[sectionCount];
    this.z2 = new double[sectionCount];
  }


  public static double[] lowpass(double cutoff, double samplerate, double Q) {
    var K    = Math.tan(Math.PI * cutoff / samplerate);
    var norm = 1 / (1 + K / Q + K * K);
    var a0   = K * K * norm;
    var a1   = 2 * a0;
    var a2   = a0;
    var b1   = 2 * (K * K - 1) * norm;
    var b2   = (1 - K / Q + K * K) * norm;
    return { a0, a1, a2, b1, b2 };
  }

  public static double[] highpass(double cutoff, double samplerate, double Q) {
    var K    = Math.tan(Math.PI * cutoff / samplerate);
    var norm = 1 / (1 + K / Q + K * K);
    var a0   = 1 * norm;
    var a1   = -2 * a0;
    var a2   = a0;
    var b1   = 2 * (K * K - 1) * norm;
    var b2   = (1 - K / Q + K * K) * norm;
    return { a0, a1, a2, b1, b2 };
  }

  public static double[] bandpass(double cutoff, double samplerate, double Q) {
    var K    = Math.tan(Math.PI * cutoff / samplerate);
    var norm = 1 / (1 + K / Q + K * K);
    var a0   = K / Q * norm;
    var a1   = 0;
    var a2   = -a0;
    var b1   = 2 * (K * K - 1) * norm;
    var b2   = (1 - K / Q + K * K) * norm;
    return { a0, a1, a2, b1, b2 };
  }




  /* Delay line */
  double[] z1;
  double[] z2;

  public void reset() {
    for (var i = 0; i < z1.length; ++i) {
      z1[i] = 0.0;
      z2[i] = 0.0;
    }
  }

  public void filter(double[] biquad, float[] input, float[] output) {
    if (biquad.length < 5) return;

    var y = 0.0, x = 0.0;

    /* IIR filtering - cascaded biquads */
    for (var i = 0; i < input.length; ++i) {
      x = input[i];

      /* Cascade */
      for (int j = 0; j < z1.length; ++j) {
        /*
          Transposed direct form II
            z2   = a2 * x[n-2] – b2 * y[n-2]
            z1   = a1 * x[n-1] – b1 * y[n-1] + z2
            y[n] = a0 * input[i]
        */
        y     = x * biquad[0] + z1[j];
        z1[j] = x * biquad[1] + z2[j] - biquad[3] * y;    //  biquad[2] -> biquad[1] ????????
        z2[j] = x * biquad[2] - biquad[4] * y;
        x     = y;
      }
      output[i] = (float) y;
    }
  }

}