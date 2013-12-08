/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "Cepstrum.h"
#include "utils.h"
#include "ffts.h"
#include "findLag.h"

#define PI  3.14159265358979323846264338327950288419716939937510582097494459230


inline static void blackman(float* response, int length) {

  const int last = length - 1;
  const float factor = 2.0 * PI / last;
  const float alpha = 0.42;
  const float beta = 0.5;
  const float gamma = 0.08;

  for (int i = 0; i <= last; ++i) {
    response[i] = alpha - beta * cos(i * factor) + gamma * cos(2 * i * factor);
  }

}


Cepstrum* Cepstrum_create(int samplerate, int windowSize) {

  Cepstrum* self = malloc(sizeof(Cepstrum));
  assert(self != NULL);

  self->samplerate = samplerate;

  // The output of a real-to-complex transform is N/2+1 complex numbers
  int fftBinCount = windowSize + 1;

  self->fftPlan  = ffts_init_1d_real(windowSize, -1);
  self->ifftPlan = ffts_init_1d_real(windowSize, 1);

  self->fft      = CpxFloatArray_create(fftBinCount);
  self->audio    = FloatArray_create(windowSize);
  self->window   = FloatArray_create(windowSize);
  self->spectrum = FloatArray_create(fftBinCount);

  blackman(self->window.elements, windowSize);
  FloatArray_fill(self->audio, 0.0);

  return self;

}


void Cepstrum_destroy(Cepstrum* self) {

  FloatArray_destroy(self->audio);
  CpxFloatArray_destroy(self->fft);
  ffts_free(self->fftPlan);
  ffts_free(self->ifftPlan);
  FloatArray_destroy(self->window);
  FloatArray_destroy(self->spectrum);
  ffts_free(self->cepPlan);
  free(self);
  self = NULL;

}


void Cepstrum_estimate(Cepstrum* self, float* data, float *outFreq, float *outAmp) {

  // padded audio data
  memcpy(self->audio.elements, data, self->audio.length * sizeof(float));

  // Blackman
  for (int i = 0; i < self->window.length; ++i) {
    // self->audio.elements[i] *= self->window.elements[i];
  }

  // FFT
  ffts_execute(self->fftPlan, self->audio.elements, self->fft.elements);

  // log power spectrum
  for (int i = 0; i < self->fft.length; ++i) {
    self->spectrum.elements[i] = log10(1.0 + creal(magnitude(self->fft.elements[i])));
  }

  ffts_execute(self->cepPlan, self->spectrum.elements, self->fft.elements);

  // log power cepstrum
  int n = self->fft.length/2;
  for (int i = 0; i < n; ++i) {
    self->spectrum.elements[i] = log10(creal(magnitude(self->fft.elements[i])));
  }

  float amp, lag;
  // findLag(self->spectrum.elements, n, 0.5, &lag, &amp);

  // *outFreq = (lag > 2.0) ? self->samplerate / lag : 0.0; // anything less than 2 is probably an error
  // *outAmp = (amp >= 0.0) ? amp : 0.0;

  // printf("    %8.4f     %4.2f          \r", *outFreq, *outAmp); fflush(stdout);

}


#undef PI
