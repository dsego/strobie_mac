// Copyright (c) Davorin Šego. All rights reserved.

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "config.h"



Config* Config_create() {

  Config* self = malloc(sizeof(Config));
  assert(self != NULL);

  self->inputDevice        = 0;
  self->inputBufferSize    = 0;         // auto-detect
  self->maxInputBufferSize = 1024;      // buffers are allocated only once
  self->outputDevice       = 0;
  self->samplerate         = 44100;
  self->windowSize         = 2048;
  self->pitchStandard      = 440;
  self->displayFlats       = 0;
  self->centsOffset        = 0;
  self->transpose          = 0;
  self->freq               = 82.4;
  self->clarityThreshold   = 0.8;
  self->gain               = 100;
  self->highlightBands     = 0;


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

  self->schemeIndex = 0;
  self->schemeCount = 6;

  strcpy(self->schemes[0].name, "Default");
  self->schemes[0].a[0] = 181;
  self->schemes[0].a[1] = 242;
  self->schemes[0].a[2] = 219;
  self->schemes[0].b[0] = 107;
  self->schemes[0].b[1] = 61;
  self->schemes[0].b[2] = 125;

  strcpy(self->schemes[1].name, "Red");
  self->schemes[1].a[0] = 255;
  self->schemes[1].a[1] = 98;
  self->schemes[1].a[2] = 80;
  self->schemes[1].b[0] = 111;
  self->schemes[1].b[1] = 34;
  self->schemes[1].b[2] = 37;

  strcpy(self->schemes[2].name, "Vintage");
  self->schemes[2].a[0] = 245;
  self->schemes[2].a[1] = 224;
  self->schemes[2].a[2] = 132;
  self->schemes[2].b[0] = 107;
  self->schemes[2].b[1] = 73;
  self->schemes[2].b[2] = 68;

  strcpy(self->schemes[3].name, "Copper");
  self->schemes[3].a[0] = 240;
  self->schemes[3].a[1] = 124;
  self->schemes[3].a[2] = 86;
  self->schemes[3].b[0] = 112;
  self->schemes[3].b[1] = 41;
  self->schemes[3].b[2] = 19;

  strcpy(self->schemes[4].name, "Gray");
  self->schemes[4].a[0] = 240;
  self->schemes[4].a[1] = 240;
  self->schemes[4].a[2] = 240;
  self->schemes[4].b[0] = 80;
  self->schemes[4].b[1] = 80;
  self->schemes[4].b[2] = 80;

  strcpy(self->schemes[5].name, "Custom...");
  self->schemes[5].a[0] = 240;
  self->schemes[5].a[1] = 240;
  self->schemes[5].a[2] = 240;
  self->schemes[5].b[0] = 80;
  self->schemes[5].b[1] = 80;
  self->schemes[5].b[2] = 80;

  return self;

}


void Config_destroy(Config* self) {

  free(self);
  self = NULL;

}
