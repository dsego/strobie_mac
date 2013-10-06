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
  // self->inputDevice  = 2;
  self->outputDevice = 0;
  self->samplerate   = 44100;

  self->fftLength      = 4096;
  self->pitchStandard  = 440;
  self->displayFlats   = 0;
  self->centsOffset    = 0;
  self->transpose      = 0;
  self->audioThreshold = -60;

  self->reference = Tuning12TET_find(110, self->pitchStandard, self->centsOffset);

  int samplesPerPeriod = 512;
  float periodsPerFrame = 1;

  self->strobeCount = 5;

  for (int i = 0; i < self->strobeCount; ++i) {

    self->strobes[i].color1[0] = 217;
    self->strobes[i].color1[1] = 255;
    self->strobes[i].color1[2] = 163;

    self->strobes[i].color2[0] = 28;
    self->strobes[i].color2[1] = 80;
    self->strobes[i].color2[2] = 161;

    self->strobes[i].samplesPerPeriod = samplesPerPeriod;
    self->strobes[i].periodsPerFrame = periodsPerFrame;
    self->strobes[i].bufferLength = 512;
    self->strobes[i].resampledLength = 1024;
    self->strobes[i].centsOffset = 0;
    self->strobes[i].gain = 1000;
    self->strobes[i].mode = OCTAVE;
    self->strobes[i].value = i + 1;
    self->strobes[i].subdivCount = 0;

    samplesPerPeriod /= 2;
    periodsPerFrame *= 2;

  }

  return self;

}


void Config_destroy(Config* self) {

  free(self);
  self = NULL;

}