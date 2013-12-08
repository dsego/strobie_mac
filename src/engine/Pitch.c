/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "Pitch.h"



Pitch* Pitch_create(EstimationMethod method, int samplerate, int windowSize) {

  Pitch* self = malloc(sizeof(Pitch));
  assert(self != NULL);

  self->method = method;
  switch (method) {
    case CEPSTRUM_METHOD:
      self->estimator = (void*)Cepstrum_create(samplerate, windowSize);
      break;
    case NSDF_METHOD:
      self->estimator = (void*)NSDF_create(samplerate, windowSize);
      break;
    case HPS_METHOD:
      // self->hps = HPS_create(samplerate, windowSize);
      break;
  }

  return self;

}


void Pitch_destroy(Pitch* self) {

  switch (self->method) {
    case CEPSTRUM_METHOD:
      Cepstrum_destroy((Cepstrum*)self->estimator);
      break;
    case NSDF_METHOD:
      NSDF_destroy((NSDF*)self->estimator);
      break;
    case HPS_METHOD:
      // HPS_destroy();
      break;
  }

  free(self);
  self = NULL;

}


void Pitch_estimate(Pitch* self, float* data, float *outFreq, float *outClarity) {

  switch (self->method) {
    case CEPSTRUM_METHOD:
      Cepstrum_estimate((Cepstrum*)self->estimator, data, outFreq, outClarity);
      break;
    case NSDF_METHOD:
      NSDF_estimate((NSDF*) self->estimator, data, outFreq, outClarity);
      break;
    case HPS_METHOD:
      // HPS_destroy();
      break;
  }

}
