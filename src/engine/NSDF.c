/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "NSDF.h"
#include "utils.h"
#include "ffts.h"
#include "findLag.h"


NSDF* NSDF_create(int samplerate, int windowSize) {

  NSDF* self = malloc(sizeof(NSDF));
  assert(self != NULL);

  self->samplerate = samplerate;

  // zero-pad (autocorrelation)
  int paddedLength = windowSize + windowSize;

  // The output of a real-to-complex transform is N/2+1 complex numbers
  int fftBinCount = windowSize + 1;

  self->fftPlan  = ffts_init_1d_real(paddedLength, -1);
  self->ifftPlan = ffts_init_1d_real(paddedLength, 1);
  self->fft      = CpxFloatArray_create(fftBinCount);
  self->audio    = FloatArray_create(paddedLength);
  self->nsdf     = FloatArray_create(paddedLength);

  return self;

}


void NSDF_destroy(NSDF* self) {

  FloatArray_destroy(self->audio);
  FloatArray_destroy(self->nsdf);
  CpxFloatArray_destroy(self->fft);
  ffts_free(self->fftPlan);
  ffts_free(self->ifftPlan);
  free(self);
  self = NULL;

}


void NSDF_estimate(NSDF* self, float* data, float *outFreq, float *outAmp) {

  // pad audio data
  memcpy(self->audio.elements, data, (self->audio.length / 2) * sizeof(float));

  // autocorrelation:  FFT -> power spectrum -> inverse FFT
  ffts_execute(self->fftPlan, self->audio.elements, self->fft.elements);

  for (int i = 0; i < self->fft.length; ++i) {
    self->fft.elements[i] = magnitude(self->fft.elements[i]);
  }

  ffts_execute(self->ifftPlan, self->fft.elements, self->nsdf.elements);

  int n = self->nsdf.length / 2;

  // TODO - parameter to constructor (?)
  int k = n - 256;
  double norm = 1.0 / (double) self->nsdf.length;

  float *audio = self->audio.elements;

  // left-hand summation for zero lag
  double lhsum = 2.0 * (self->nsdf.elements[0] * norm);

  // normalized SDF (via autocorrelation)
  for (int i = 0; i < k; ++i) {

    if (lhsum > 0.0) {
      self->nsdf.elements[i] *= norm * 2.0 / lhsum;
    }
    else {
      self->nsdf.elements[i] = 0.0;
    }

    lhsum -= audio[i] * audio[i] + audio[n-i-1] * audio[n-i-1];

  }

  float amp, lag;
  findLag(self->nsdf.elements, k, 0.9, &lag, &amp);

  *outFreq = (lag > 2.0) ? self->samplerate / lag : 0.0; // anything less than 2 is probably an error
  *outAmp = (amp >= 0.0) ? amp : 0.0;

}

