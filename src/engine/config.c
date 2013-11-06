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

  // minimum detectable frequency = sample rate / FFT size
  self->fftLength     = 2048;

  self->pitchStandard = 440;
  self->displayFlats  = 0;
  self->centsOffset   = 0;
  self->transpose     = 0;
  self->freq          = 82.4;

  self->pitchDetectionUpperThreshold = 0.01;
  self->pitchDetectionLowerThreshold = 0.005;

  int samplesPerPeriod = 512;
  float periodsPerFrame = 1;

  self->strobeCount = 5;

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
