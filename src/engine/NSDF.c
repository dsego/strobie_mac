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
  self->fft      = Vec_create(fftBinCount, sizeof(float complex));
  self->audio    = Vec_create(paddedLength, sizeof(float));
  self->nsdf     = Vec_create(paddedLength, sizeof(float));

  // fill with zeros
  float *audio = (float*) self->audio.elements;
  for (int i = 0; i < self->audio.count; ++i) {
    audio[i] = 0.0;
  }

  return self;

}


void NSDF_destroy(NSDF* self) {

  Vec_destroy(self->audio);
  Vec_destroy(self->nsdf);
  Vec_destroy(self->fft);
  ffts_free(self->fftPlan);
  ffts_free(self->ifftPlan);
  free(self);
  self = NULL;

}


void NSDF_estimate(NSDF* self, const float* data, float *outFreq, float *outAmp) {

  float complex *fft = (float complex *) self->fft.elements;
  float *nsdf = (float *) self->nsdf.elements;
  float *audio = (float *) self->audio.elements;

  // pad audio data
  memcpy(audio, data, (self->audio.count / 2) * sizeof(float));

  // autocorrelation:  FFT -> power spectrum -> inverse FFT
  ffts_execute(self->fftPlan, audio, fft);

  for (int i = 0; i < self->fft.count; ++i) {
    fft[i] = magnitude(fft[i]);
  }

  ffts_execute(self->ifftPlan, fft, nsdf);

  int n = self->nsdf.count / 2;

  // TODO - parameter to constructor (?)
  int k = n - 256;
  double norm = 1.0 / (double) self->nsdf.count;


  // left-hand summation for zero lag
  double lhsum = 2.0 * (nsdf[0] * norm);

  // normalized SDF (via autocorrelation)
  for (int i = 0; i < k; ++i) {

    if (lhsum > 0.0) {
      nsdf[i] *= norm * 2.0 / lhsum;
    }
    else {
      nsdf[i] = 0.0;
    }

    lhsum -= audio[i] * audio[i] + audio[n-i-1] * audio[n-i-1];

  }

  float amp, lag;
  findLag(nsdf, k, 0.9, &lag, &amp);

  *outFreq = (lag > 2.0) ? self->samplerate / lag : 0.0; // anything less than 2 is probably an error
  *outAmp = (amp >= 0.0) ? amp : 0.0;

}

