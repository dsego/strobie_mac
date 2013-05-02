/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "AutocorrPitch.h"
#include "kiss_fftr.h"


AutocorrPitch* AutocorrPitch_create(int samplerate, int fftLength) {

  AutocorrPitch* self = malloc(sizeof(AutocorrPitch));
  assert(self != NULL);

  // padded FFT frame
  self->fftLength  = fftLength + fftLength / 2;

  self->samplerate = samplerate;
  self->binCount   = self->fftLength / 2 + 1;

  self->fftCfg  = kiss_fftr_alloc(self->fftLength, 0, NULL, NULL);
  self->ifftCfg = kiss_fftr_alloc(self->fftLength, 1, NULL, NULL);

  self->fftData = malloc(self->binCount * sizeof(kiss_fft_cpx));
  assert(self->fftData != NULL);

  self->paddedData = calloc(self->fftLength, sizeof(float));
  assert(self->paddedData != NULL);

  self->autocorrData = malloc(self->fftLength * sizeof(float));
  assert(self->autocorrData != NULL);

  return self;

}


void AutocorrPitch_destroy(AutocorrPitch* self) {

  kiss_fftr_free(self->fftCfg);
  kiss_fftr_free(self->ifftCfg);
  free(self->fftData);
  free(self->paddedData);
  free(self->autocorrData);
  free(self);
  self = NULL;

}


// power spectrum  (a + bi)(a - bi) = (aa + bb) + (ab - ab)i
void AutocorrPitch_powerSpectrum(kiss_fft_cpx* bins, int binsLength) {

  for (int i = 0; i < binsLength; ++i) {
    bins[i].r = bins[i].r * bins[i].r + bins[i].i * bins[i].i;
    bins[i].i = 0;
  }

}


// Quadratic Interpolation of Spectral Peaks
//   https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html */
double AutocorrPitch_parabolaPeakLocation(double y0, double y1, double y2) {

  return 0.5 * (y0 - y2) / (y0 - 2.0 * y1 + y2);

}


void AutocorrPitch_normCenterClip(float* data, int dataLength) {

  double max = 0;

  // find absolute max
  for (int i = 0; i < dataLength; ++i) {
    if (data[i] > max)
      max = data[i];
    else if (data[i] < -max)
      max = -data[i];
  }

  double k = 1.0 / max;

  for (int i = 0; i < dataLength; ++i) {
    data[i] *= k;
    if (-0.5 < data[i] && data[i] < 0.5) {
      data[i] = 0;
    }
  }

}


double AutocorrPitch_estimate(AutocorrPitch* self, float* data, int dataLength) {

  // important to get more consistent and accurate results!
  //
  //
  // (NO REALLY SURE, MORE TESTING NEEDED)
  //
  //
  // AutocorrPitch_normCenterClip(data, dataLength);



  // zero pad to avoid aliasing caused by the terms wrapping around
  memcpy(self->paddedData, data, dataLength * sizeof(float));

  // forward Fourier transform
  kiss_fftr(self->fftCfg, self->paddedData, self->fftData);

  // correlation
  AutocorrPitch_powerSpectrum(self->fftData, self->binCount);

  // inverse Fourier
  kiss_fftri(self->ifftCfg, self->fftData, self->autocorrData);

  // ramp
  for (int i = 1; i < self->fftLength / 2; ++i) {
    self->autocorrData[i] *= 1.0 + (self->fftLength - i) / self->fftLength;
  }

  int low = 0, x = 0;

  // find lowest point
  for (int i = 2; i < self->fftLength / 2; ++i) {
    if (self->autocorrData[i] > self->autocorrData[i - 1]) {
      low = i - 1;
      break;
    }
  }

  double threshold = self->autocorrData[low];

  // find peak (period)
  for (int i = low; i < self->fftLength / 2 - 1; ++i) {

    if (self->autocorrData[i] > threshold) {
      // emphasize peaks at shorter lags
      threshold = self->autocorrData[i] * 1.25;
      x = i;
    }

  }

  double pitch = 0;

  // estimate true peak via parabolic interpolation
  double truePeak = x + AutocorrPitch_parabolaPeakLocation(
    self->autocorrData[x - 1],
    self->autocorrData[x],
    self->autocorrData[x + 1]
  );

  if (truePeak != 0) {
    pitch = self->samplerate / truePeak;
  }

  return pitch;

}