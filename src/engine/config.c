/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "Config.h"



Config* Config_create() {

  Config* self = malloc(sizeof(Config));
  assert(self != NULL);

  // general
  self->deviceIndex      = 0;
  self->gain             = 1000.0;
  self->samplerate       = 44100;
  self->bufferLength     = 512;
  self->resampledLength  = 512 * 32;

  // pitch recognition
  self->fftLength           = 2048;
  self->audioThreshold      = -60;
  self->pitchStandard       = 440;
  self->displayFlats        = 0;
  self->transpose           = 0;
  self->centsOffset         = 0;

  // strobe bands
  self->strobeColor1[0]     = 253;
  self->strobeColor1[1]     = 151;
  self->strobeColor1[2]     = 32;
  self->strobeColor2[0]     = 43;
  self->strobeColor2[1]     = 33;
  self->strobeColor2[2]     = 16;
  self->periodsPerFrame     = 1;
  self->partialsCount       = 3;
  self->partials[0]         = 1;
  self->partials[1]         = 2;
  self->partials[2]         = 3;
  self->partials[3]         = 4;
  self->partials[4]         = 5;
  self->partials[5]         = 6;
  self->partials[6]         = 7;
  self->samplesPerPeriod[0] = 256;
  self->samplesPerPeriod[1] = 256;
  self->samplesPerPeriod[2] = 256;
  self->samplesPerPeriod[3] = 256;
  self->samplesPerPeriod[4] = 256;
  self->samplesPerPeriod[5] = 256;
  self->samplesPerPeriod[6] = 256;

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