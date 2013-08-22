/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "Config.h"



Config* Config_create() {

  Config* self = malloc(sizeof(Config));
  assert(self != NULL);

  // general
  self->deviceIndex            = 2;
  self->samplerate             = 44100;
  self->bufferLength           = 512;
  self->resampledBufferLength  = 512 * 25;

  // pitch recognition
  self->averageCount        = 0;
  self->decimationRate      = 1;
  self->fftSamplerate       = self->samplerate / self->decimationRate;
  self->fftLength           = 2048;
  self->audioThreshold      = -60;
  self->pitchStandard       = 440;
  self->displayFlats        = false;
  self->transpose           = 0;
  self->centsOffset         = 0;

  // strobe bands
  self->periodsPerFrame     = 1;
  self->partialsLength      = 3;
  self->partials[0]         = 1;
  self->partials[1]         = 2;
  self->partials[2]         = 4;
  self->samplesPerPeriod[0] = 64;
  self->samplesPerPeriod[1] = 128;
  self->samplesPerPeriod[2] = 256;

  // standard guitar tuning (in cents)
  self->instrumentNotes[0] = -2900;
  self->instrumentNotes[1] = -2400;
  self->instrumentNotes[2] = -1900;
  self->instrumentNotes[3] = -1400;
  self->instrumentNotes[4] = -1000;
  self->instrumentNotes[5] = -500;

  return self;

}


void Config_destroy(Config* self) {

  free(self);
  self = NULL;

}