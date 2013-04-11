/*
  Copyright (C) 2013 Davorin Šego
*/

public class PitchEstimation {

  float pitch = 0.0f;

  KissFFT.Cpx[] fft1;
  KissFFT.Cpx[] fft2;

  KissFFTR.Cfg fft1_cfg;
  KissFFTR.Cfg fft2_cfg;
  KissFFTR.Cfg ffti_cfg;

  int sample_rate;
  int fft1_length;

  public float[] padded_data;
  public float[] autocorr_data;
  public float[] cepstrum;
  public float[] spectrum;




  public PitchEstimation(int sample_rate, int data_length) {
    this.sample_rate = sample_rate;
    this.fft1_length  = data_length;
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
    /* normalize & center clip */
    normalize_and_center_clip(data);

    /* pad with zeros */
    Posix.memcpy(padded_data, data, data.length * sizeof(float));

    /* FFT the time domain signal */
    KissFFTR.transform(fft1_cfg, padded_data, fft1);

    /* power spectrum  (a + bi)(a - bi) = (aa + bb) + (ab - ab)i */
    for (int i = 0; i < fft1.length; ++i) {
      fft1[i].r = fft1[i].r * fft1[i].r + fft1[i].i * fft1[i].i;
      fft1[i].i = 0;
    }

    /* inverse FFT */
    KissFFTR.transform_inverse(ffti_cfg, fft1, autocorr_data);

    /* ramp */
    for (int i = 1; i < autocorr_data.length / 2; ++i) {
      autocorr_data[i] *= 1f + (float) (autocorr_data.length - i) / autocorr_data.length;
    }

    int low = 0, x = 0;

    for (int i = 2; i < autocorr_data.length / 2; ++i) {
      if (autocorr_data[i] > autocorr_data[i - 1]) {
        low = i - 1;
        break;
      }
    }

    var threshold = 0.7 * autocorr_data[0];

    for (int i = low; i < autocorr_data.length / 2; ++i) {
      if (autocorr_data[i] > threshold) {
        threshold = autocorr_data[i];
        x = i;
      }
    }

    if (x != 0) {
      /* Interpolation */
      // pitch = SRC.cubic(autocorr_data[x - 2], autocorr_data[x - 1], autocorr_data[x], autocorr_data[x + 1], double t);

      pitch = x + parabolic(autocorr_data[x - 1], autocorr_data[x], autocorr_data[x + 1]);
      pitch = (float) (this.sample_rate / pitch);
    }

    return pitch;
  }


  public float pitch_from_hps(float[] data) {
    return 0.0f;
  }


  public float pitch_from_fft(float[] data) {
    /* normalize & center clip */
    normalize_and_center_clip(data);

    Window.gaussian(data);
    // Window.hann(data);
    KissFFTR.transform(fft1_cfg, data, fft1);

    /* spectrum */
    // for (int i = 1; i < fft1.length; ++i) {
    for (int i = 0; i < fft1.length; ++i) {
      // spectrum[i] = (float) Math.log10(Math.sqrt(fft1[i].r * fft1[i].r + fft1[i].i * fft1[i].i));
      spectrum[i] = (float) (fft1[i].r * fft1[i].r + fft1[i].i * fft1[i].i);
    }

    int p = 0;
    float threshold = 0f;

    for (int i = 1; i < spectrum.length; ++i) {
      if (spectrum[i] > threshold) {
        threshold = spectrum[i];
        p = i;
      }
    }

    var ip = parabolic(spectrum[p - 1], spectrum[p], spectrum[p + 1]);
    return (float) (p + ip) * sample_rate / fft1_length;
  }



  /* Quadratic Interpolation of Spectral Peaks
      https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html */
  private static float parabolic(float y0, float y1, float y2) {
    return (float) (0.5 * (y0 - y2) / (y0 - 2 * y1 + y2));
  }

  private void normalize_and_center_clip(float[] data) {
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
      if (-0.5f < data[i] < 0.5f) data[i] = 0;
    }
  }

}