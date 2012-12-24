/*
  Copyright (C) 2012 Davorin Šego
*/

public class PitchEstimation {

  KissFFT.Cpx[] fft1;
  KissFFT.Cpx[] fft2;
  KissFFTR.Cfg fft1_cfg;
  KissFFTR.Cfg fft2_cfg;
  KissFFTR.Cfg ffti_cfg;
  int sample_rate;
  int fft1_length;
  public float[] padded_data;
  public float[] autocorr_data;

  public weak float[] autocorrelation;
  public float[] cepstrum;
  public float[] spectrum;

  public PitchEstimation(int sample_rate, int data_length) {
    this.sample_rate = sample_rate;
    // this.fft1_length  = data_length * 2;
    this.fft1_length  = data_length;
    // spectrum         = new float[data_length + 1];
    // fft              = new KissFFT.Cpx[data_length + 1];
    spectrum      = new float[data_length / 2 + 1];
    cepstrum      = new float[data_length / 4 + 1];
    fft1          = new KissFFT.Cpx[data_length / 2 + 1];
    fft2          = new KissFFT.Cpx[data_length / 4 + 1];
    fft1_cfg      = KissFFTR.alloc(fft1_length, false, null, null);
    fft2_cfg      = KissFFTR.alloc(fft1_length / 2, false, null, null);
    ffti_cfg      = KissFFTR.alloc(fft1_length, true, null, null);
    padded_data   = new float[data_length * 2];
    autocorr_data = new float[data_length];
  }

  public float pitch_from_autocorrelation(float[] data) {

    autocorrelation        = autocorr_data;
    autocorrelation.length = autocorr_data.length / 2;

    weak float[] autocorr = autocorr_data;
    autocorr.length       = autocorr_data.length / 2;

    /* Center clip */

    center_clip(data);

    /* Pad with zeros */
    Posix.memcpy(padded_data, data, data.length * sizeof(float));

    /* FFT the time domain signal */
    KissFFTR.transform(fft1_cfg, padded_data, fft1);

    /* power spectrum */
    /* (a + bi)(a - bi) = (aa + bb) + (ab - ab)i */
    for (int i = 0; i < fft1.length; ++i) {
      fft1[i].r = fft1[i].r * fft1[i].r + fft1[i].i * fft1[i].i;
      fft1[i].i = 0;
    }

    /* inverse FFT */
    KissFFTR.transform_inverse(ffti_cfg, fft1, autocorr_data);

    for (int i = 1; i < autocorr.length; ++i) {
      /* apply positive ramp */
      autocorr[i] *= 1f + (float) i / autocorr.length;
    }

    int x1 = 0, x2 = 0, low = 0;
    float x = 0f;

    for (int i = 1; i < autocorr.length; ++i) {
      if (autocorr[i] > autocorr[i - 1]) {
        low = i - 1;
        break;
      }
    }

    var threshold = 0.5 * autocorr[0];

    for (int i = low; i < autocorr.length - 1; ++i) {
      if (autocorr[i] >= threshold && autocorr[i + 1] < autocorr[i]) {
        x = i + 0.5f * (autocorr[i - 1] - autocorr[i + 1]) / (autocorr[i - 1] - 2 * autocorr[i] + autocorr[i + 1]);
        break;
      }
    }

    // return x1;

    if (x == 0) {
      return 0;
    } else {
      return (float) (44100.0 / x);
    }

    return 0;
  }

  public float pitch_from_fft(float[] data) {
    Window.gaussian(data);
    // Window.hann(data);
    KissFFTR.transform(fft1_cfg, data, fft1);

    /* spectrum */
    for (int i = 1; i < fft1.length; ++i) {
      // spectrum[i] = (float) Math.log10(Math.sqrt(fft1[i].r * fft1[i].r + fft1[i].i * fft1[i].i));
      // spectrum[i] = (float) (fft1[i].r * fft1[i].r + fft1[i].i * fft1[i].i);
    }

    int p = 0;
    float threshold = 0f;

    for (int i = 1; i < spectrum.length; ++i) {
      if (spectrum[i] > threshold) {
        threshold = spectrum[i];
        p = i;
      }
    }

    /* Interpolated peak */
    var ip = 0.5 * (spectrum[p - 1] - spectrum[p + 1]) / (spectrum[p - 1] - 2 * spectrum[p] + spectrum[p + 1]);
    return (float) (p + ip) * sample_rate / fft1_length;
  }


  private void center_clip(float[] data) {
    var amp = 0f;
    for (int i = 1; i < data.length; ++i) {
      if (data[i] > amp) {
        amp = data[i];
      } else if (data[i] < -amp) {
        amp = -data[i];
      }
    }

    amp *= 0.5f;
    for (int i = 1; i < data.length; ++i) {
      if (data[i] > amp) {
        data[i] -= amp;
      } else if (data[i] < -amp) {
        data[i] += amp;
      } else {
        data[i] = 0;
      }
    }
  }

}