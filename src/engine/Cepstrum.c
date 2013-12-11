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
  self->fft      = Vec_create(fftBinCount, sizeof(complex float));
  self->audio    = Vec_create(windowSize, sizeof(float));
  self->window   = Vec_create(windowSize, sizeof(float));
  self->spectrum = Vec_create(fftBinCount, sizeof(float));

  blackman((float*)self->window.elements, windowSize);

  // fill with zeros
  float *audio = (float*) self->audio.elements;
  for (int i = 0; i < self->audio.count; ++i) {
    audio[i] = 0.0;
  }

  return self;

}


void Cepstrum_destroy(Cepstrum* self) {

  Vec_destroy(self->audio);
  Vec_destroy(self->fft);
  Vec_destroy(self->window);
  Vec_destroy(self->spectrum);
  ffts_free(self->fftPlan);
  ffts_free(self->ifftPlan);
  ffts_free(self->cepPlan);
  free(self);
  self = NULL;

}


void Cepstrum_estimate(Cepstrum* self, float* data, float *outFreq, float *outAmp) {

  float complex* fft = (float complex *) self->fft.elements;
  float* audio = (float*) self->audio.elements;
  float* spectrum = (float*) self->spectrum.elements;

  // padded audio data
  memcpy(audio, data, self->audio.count * sizeof(float));

  // Blackman
  for (int i = 0; i < self->window.count; ++i) {
    // self->audio.elements[i] *= self->window.elements[i];
  }

  // FFT
  ffts_execute(self->fftPlan, audio, fft);

  // log power spectrum
  for (int i = 0; i < self->fft.count; ++i) {
    spectrum[i] = log10(1.0 + creal(magnitude(fft[i])));
  }

  ffts_execute(self->cepPlan, spectrum, fft);

  // log power cepstrum
  int n = self->fft.count/2;
  for (int i = 0; i < n; ++i) {
    spectrum[i] = log10(creal(magnitude(fft[i])));
  }

  // float amp, lag;
  // findLag(self->spectrum.elements, n, 0.5, &lag, &amp);

  // *outFreq = (lag > 2.0) ? self->samplerate / lag : 0.0; // anything less than 2 is probably an error
  // *outAmp = (amp >= 0.0) ? amp : 0.0;

  // printf("    %8.4f     %4.2f          \r", *outFreq, *outAmp); fflush(stdout);

}


#undef PI
