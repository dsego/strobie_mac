/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "Config.h"



Config* Config_create() {

  Config* self = malloc(sizeof(Config));
  assert(self != NULL);

  self->inputDevice      = 0;
  self->inputBufferSize  = 0;  // auto-detect
  self->outputDevice     = 0;
  self->samplerate       = 44100;
  self->windowSize       = 2048;

  self->pitchStandard = 440;
  self->displayFlats  = 0;
  self->centsOffset   = 0;
  self->transpose     = 0;
  self->freq          = 82.4;
  self->peakThreshold = 0.01;
  self->clarityThreshold = 0.8;
  self->gain = 1000;

  int samplesPerPeriod = 512;
  float periodsPerFrame = 1;

  self->strobeCount = 6;

  for (int i = 0; i < self->strobeCount; ++i) {

    self->strobes[i].color1[0] = 217;
    self->strobes[i].color1[1] = 254;
    self->strobes[i].color1[2] = 163;

    self->strobes[i].color2[0] = 27;
    self->strobes[i].color2[1] = 79;
    self->strobes[i].color2[2] = 161;

    self->strobes[i].samplesPerPeriod = samplesPerPeriod;
    self->strobes[i].periodsPerFrame = periodsPerFrame;
    self->strobes[i].bufferLength = 512;
    self->strobes[i].resampledLength = 1024;
    self->strobes[i].centsOffset = 0;
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
