/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "Config.h"



Config* Config_create() {

  Config* self = malloc(sizeof(Config));
  assert(self != NULL);

  self->inputDevice  = 0;
  self->outputDevice = 0;
  self->samplerate   = 44100;

  self->fftLength      = 2048;
  self->pitchStandard  = 440;
  self->displayFlats   = 0;
  self->centsOffset    = 0;
  self->transpose      = 0;
  self->audioThreshold = -60;

  self->reference = Tuning12TET_find(440.0, self->pitchStandard, self->centsOffset, self->transpose);

  int samplesPerPeriod = 1024;
  float periodsPerFrame = 0.5;

  self->strobeCount = 8;

  for (int i = 0; i < self->strobeCount; ++i) {

    self->strobes[i].color1[0] = 253;
    self->strobes[i].color1[1] = 151;
    self->strobes[i].color1[2] = 32;

    self->strobes[i].color2[0] = 43;
    self->strobes[i].color2[1] = 33;
    self->strobes[i].color2[2] = 16;

    self->strobes[i].samplesPerPeriod = samplesPerPeriod;
    self->strobes[i].periodsPerFrame = periodsPerFrame;
    self->strobes[i].bufferLength = 512;
    self->strobes[i].resampledLength = 512;
    self->strobes[i].centsOffset = 0;
    self->strobes[i].gain = 100;
    self->strobes[i].mode = OCTAVE;
    self->strobes[i].value = i;

    samplesPerPeriod /= 2;
    periodsPerFrame *= 2;

  }

  return self;

}


void Config_destroy(Config* self) {

  free(self);
  self = NULL;

}