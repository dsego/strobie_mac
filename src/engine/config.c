/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "Config.h"



Config* Config_create() {

  Config* self = malloc(sizeof(Config));
  assert(self != NULL);

  self->inputDevice        = 0;
  self->inputBufferSize    = 0;         // auto-detect
  self->maxInputBufferSize = 1024;      // max. input buffer size (so that buffers are allocated only once)
  self->outputDevice       = 0;
  self->samplerate         = 44100;
  self->windowSize         = 2048;
  self->pitchStandard      = 440;
  self->displayFlats       = 0;
  self->centsOffset        = 0;
  self->transpose          = 0;
  self->freq               = 82.4;
  self->peakThreshold      = 0.01;      // not used
  self->clarityThreshold   = 0.8;       // should this be user-configurable?
  self->gain               = 100;



  // 16, 32, 64, ...
  {
    int i = 0, n = 16;
    while (n <= self->maxInputBufferSize) {
      self->inputBufferSizes[i++] = n;
      n *= 2;
    }
    self->inputBufferSizes[i] = 0; // zero-terminated
  }

  int samplesPerPeriod = 512;
  float periodsPerFrame = 1;

  self->strobeCount = 6;

  for (int i = 0; i < self->strobeCount; ++i) {

    self->strobes[i].colors[0][0] = 217;
    self->strobes[i].colors[0][1] = 254;
    self->strobes[i].colors[0][2] = 163;
    self->strobes[i].colors[1][0] = 27;
    self->strobes[i].colors[1][1] = 79;
    self->strobes[i].colors[1][2] = 161;

    self->strobes[i].samplesPerPeriod = samplesPerPeriod;
    self->strobes[i].periodsPerFrame = periodsPerFrame;
    self->strobes[i].bufferLength = self->maxInputBufferSize;
    self->strobes[i].resampledLength = self->maxInputBufferSize * 4;
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
