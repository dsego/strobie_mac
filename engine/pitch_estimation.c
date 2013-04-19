/*
  Copyright (C) 2013 Davorin Šego
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "pitch_estimation.h"
#include "kiss_fftr.h"

PitchEstimation* PitchEstimation_create(int sample_rate, int fft_length)
{
  PitchEstimation* pe = malloc(sizeof(PitchEstimation)); assert(pe != NULL);
  pe->sample_rate     = sample_rate;
  pe->fft_len         = fft_length;
  pe->fft_cfg         = kiss_fftr_alloc(fft_length, 0, NULL, NULL);
  pe->ifft_cfg        = kiss_fftr_alloc(fft_length, 1, NULL, NULL);
  pe->fft             = malloc(sizeof(kiss_fft_cpx) * fft_length / 2 + 1); assert(pe->fft != NULL);
  pe->padded_data     = calloc(sizeof(float),  fft_length * 2); assert(pe->padded_data != NULL);
  pe->autocorr_data   = malloc(sizeof(float) * fft_length); assert(pe->autocorr_data != NULL);
  return pe;
}

void PitchEstimation_destroy(PitchEstimation* pe)
{
  kiss_fftr_free(pe->fft_cfg);
  kiss_fftr_free(pe->ifft_cfg);
  free(pe->fft);
  free(pe->padded_data);
  free(pe->autocorr_data);
  free(pe);
  pe = NULL;
}

void PitchEstimation_normalize_and_center_clip(float* data, int data_len)
{
  double max = 0;

  // find absolute max
  for (int i = 0; i < data_len; ++i) {
    if (data[i] > max)
      max = data[i];
    else if (data[i] < -max)
      max = -data[i];
  }
  double k = 1.0 / max;

  for (int i = 0; i < data_len; ++i) {
    data[i] *= k;
    if (-0.5 < data[i] && data[i] < 0.5)
      data[i] = 0;
  }
}

// Quadratic Interpolation of Spectral Peaks
//   https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html */
double PitchEstimation_parabolic(double y0, double y1, double y2)
{
  return 0.5 * (y0 - y2) / (y0 - 2.0 * y1 + y2);
}



double PitchEstimation_pitch_from_autocorrelation(PitchEstimation* pe, float* data, int data_len)
{
  // important to get more consistent and accurate results!
  PitchEstimation_normalize_and_center_clip(data, data_len);

  // pad with zeros
  memcpy(pe->padded_data, data, data_len * sizeof(float));

  // Fourier transform
  kiss_fftr(pe->fft_cfg, pe->padded_data, pe->fft);

  // power spectrum  (a + bi)(a - bi) = (aa + bb) + (ab - ab)i
  for (int i = 0; i < pe->fft_len; ++i) {
    pe->fft[i].r = pe->fft[i].r * pe->fft[i].r + pe->fft[i].i * pe->fft[i].i;
    pe->fft[i].i = 0;
  }

  // inverse Fourier
  kiss_fftri(pe->ifft_cfg, pe->fft, pe->autocorr_data);

  // ramp
  for (int i = 1; i < pe->fft_len / 2; ++i) {
    pe->autocorr_data[i] *= 1.0 + (pe->fft_len - i) / pe->fft_len;
  }

  int low = 0, x = 0;

  // find lowest point
  for (int i = 2; i < pe->fft_len / 2; ++i) {
    if (pe->autocorr_data[i] > pe->autocorr_data[i - 1]) {
      low = i - 1;
      break;
    }
  }

  // find peak (period)
  double threshold = 0.7 * pe->autocorr_data[0];

  for (int i = low; i < pe->fft_len / 2; ++i) {
    if (pe->autocorr_data[i] > threshold) {
      threshold = pe->autocorr_data[i];
      x = i;
    }
  }

  double pitch = 0;

  // interpolate
  if (x != 0) {
    pitch = x + PitchEstimation_parabolic(pe->autocorr_data[x - 1], pe->autocorr_data[x], pe->autocorr_data[x + 1]);
    pitch = (double) pe->sample_rate / pitch;
  }

  return pitch;
}