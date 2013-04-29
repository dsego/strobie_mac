/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "PitchEstimation.h"
#include "kiss_fftr.h"


PitchEstimation* PitchEstimation_create(int samplerate, int fftLength) {

  PitchEstimation* self = malloc(sizeof(PitchEstimation));
  assert(self != NULL);

  self->samplerate = samplerate;

  // zero pad to avoid aliasing caused by the terms wrapping around
  self->fftLength  = fftLength + fftLength / 2;

  self->fftCfg  = kiss_fftr_alloc(self->fftLength, 0, NULL, NULL);
  self->ifftCfg = kiss_fftr_alloc(self->fftLength, 1, NULL, NULL);

  self->binCount = self->fftLength / 2 + 1;
  self->fft = malloc(self->binCount * sizeof(kiss_fft_cpx));
  assert(self->fft != NULL);

  self->paddedData = calloc(self->fftLength, sizeof(float));
  assert(self->paddedData != NULL);

  self->autocorrData = malloc(self->fftLength * sizeof(float));
  assert(self->autocorrData != NULL);

  return self;

}


void PitchEstimation_destroy(PitchEstimation* self) {

  kiss_fftr_free(self->fftCfg);
  kiss_fftr_free(self->ifftCfg);
  free(self->fft);
  free(self->paddedData);
  free(self->autocorrData);
  free(self);
  self = NULL;

}


void PitchEstimation_normalizeAndCenterClip(float* data, int dataLength) {

  double max = 0;

  // find absolute max
  for (int i = 0; i < dataLength; ++i) {
    if (data[i] > max) {
      max = data[i];
    }
    else if (data[i] < -max) {
      max = -data[i];
    }
  }

  // normalization coefficient
  double k = 1.0 / max;

  // normalize & center clip
  for (int i = 0; i < dataLength; ++i) {
    data[i] = data[i] * k;
    if (-0.5 < data[i] && data[i] < 0.5) {
      data[i] = 0;
    }
  }

}

// power spectrum  (a + bi)(a - bi) = (aa + bb) + (ab - ab)i
void PitchEstimation_powerSpectrum(kiss_fft_cpx* bins, int binsLength) {

  for (int i = 0; i < binsLength; ++i) {
    bins[i].r = bins[i].r * bins[i].r + bins[i].i * bins[i].i;
    bins[i].i = 0;
  }

}


// Quadratic Interpolation of Spectral Peaks
//   https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html */
double PitchEstimation_parabolaPeakLocation(double y0, double y1, double y2) {

  return 0.5 * (y0 - y2) / (y0 - 2.0 * y1 + y2);

}


void PitchEstimation_applyHann(float* response, int responseLength) {

  int last      = responseLength - 1;
  double factor = 2.0 * M_PI / last;
  double w      = 0;

  for (int i = 0; i <= last / 2; ++i) {
    w = 0.5 * (1.0 - cos(i * factor));
    response[i] *= w;
    response[last - i] *= w;
  }

}


double PitchEstimation_fromFFT(PitchEstimation* self, float* data, int dataLength) {


  // PitchEstimation_normalizeAndCenterClip(data, dataLength);

  PitchEstimation_applyHann(data, dataLength);

  // pad with zeros
  memcpy(self->paddedData, data, dataLength * sizeof(float));

  // Fourier transform
  kiss_fftr(self->fftCfg, self->paddedData, self->fft);

  for (int i = 0; i < self->binCount; ++i) {
    self->fft[i].r = self->fft[i].r * self->fft[i].r + self->fft[i].i * self->fft[i].i;
    self->fft[i].i = 0;
  }

  double peak = 0.0;
  int bin = 0;

  for (int i = 0; i < self->binCount - 1; ++i) {
    if (self->fft[i].r > peak) {
      peak = self->fft[i].r;
      bin = i;
    }
  }

  double truePeak = bin + PitchEstimation_parabolaPeakLocation(
    log(sqrt(self->fft[bin-1].r)),
    log(sqrt(self->fft[bin].r)),
    log(sqrt(self->fft[bin+1].r))
  );

  return truePeak * self->samplerate / self->fftLength;

}


double PitchEstimation_fromAutocorrelation(
  PitchEstimation* self, float* data, int dataLength) {

  // important to get more consistent and accurate results!
  // PitchEstimation_normalizeAndCenterClip(data, dataLength);

  // pad with zeros
  memcpy(self->paddedData, data, dataLength * sizeof(float));

  // Fourier transform
  kiss_fftr(self->fftCfg, self->paddedData, self->fft);

  // power spectrum  (a + bi)(a - bi) = (aa + bb) + (ab - ab)i
  PitchEstimation_powerSpectrum(self->fft, self->binCount);

  // inverse Fourier
  kiss_fftri(self->ifftCfg, self->fft, self->autocorrData);

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
  double truePeak = x + PitchEstimation_parabolaPeakLocation(
    self->autocorrData[x - 1],
    self->autocorrData[x],
    self->autocorrData[x + 1]
  );

  self->fundamental = truePeak;

  if (truePeak != 0) {
    pitch = self->samplerate / truePeak;
  }

  return pitch;

}