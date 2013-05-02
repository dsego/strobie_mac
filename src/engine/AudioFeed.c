/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "AudioFeed.h"


AudioFeed* AudioFeed_create() {

  AudioFeed* self = malloc(sizeof(AudioFeed));
  assert(self != NULL);

  self->ringbufferData = malloc(AF_RB_LENGTH * sizeof(float));
  assert(self->ringbufferData != NULL);

  self->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(self->ringbuffer != NULL);

  PaUtil_InitializeRingBuffer(
    self->ringbuffer,
    sizeof(float),
    AF_RB_LENGTH,
    self->ringbufferData
  );

  self->filteredData = malloc(AF_FD_LENGTH * sizeof(float));
  assert(self->filteredData != NULL);

  self->decimatedData = malloc(AF_FD_LENGTH * sizeof(float));
  assert(self->decimatedData != NULL);

  self->decimationCounter = 0;
  self->decimationRate = DECIMATE_NONE;

  self->halfbandIIR  = IIR_CreateHalfbandCheby();
  self->quartbandIIR = IIR_CreateQuartbandCheby();
  self->threePercIIR = IIR_CreateThreePercCheby();

  return self;

}


void AudioFeed_setDecimationRate(AudioFeed* self, DecimationRate decimationRate) {

  self->decimationRate = decimationRate;

  switch (self->decimationRate) {

    case DECIMATE_NONE:
      self->activeIIR = NULL;
      break;

    case DECIMATE_BY_TWO:
      self->activeIIR = self->halfbandIIR;
      break;

    case DECIMATE_BY_FOUR:
      self->activeIIR = self->quartbandIIR;
      break;

    case DECIMATE_BY_THIRTY:
      self->activeIIR = self->threePercIIR;
      break;

  }

  IIR_reset(self->activeIIR);

}


void AudioFeed_destroy(AudioFeed* self) {

  free(self->halfbandIIR);
  free(self->quartbandIIR);
  free(self->threePercIIR);
  free(self->ringbufferData);
  free(self->ringbuffer);
  free(self);
  self = NULL;

}


void AudioFeed_read(AudioFeed* self, float* output, int outputLength) {

  while (PaUtil_GetRingBufferReadAvailable(self->ringbuffer)) {
    PaUtil_ReadRingBuffer(self->ringbuffer, output, outputLength);
  }

}


void AudioFeed_process(AudioFeed* self, float* input, int inputLength) {

  if (self->decimationRate != DECIMATE_NONE) {

    // alias filtering
    IIR_filter(self->activeIIR, input, self->filteredData, inputLength);

    int n = self->decimationCounter;
    int k = 0;

    // sub sample
    int limit = inputLength - self->decimationRate + 1;
    while (n < limit) {
      self->decimatedData[k] = self->filteredData[n];
      n += self->decimationRate;
      k += 1;
    }

    self->decimationCounter = inputLength - n - 1;

    PaUtil_WriteRingBuffer(self->ringbuffer, self->filteredData, k);

  }
  else {

    PaUtil_WriteRingBuffer(self->ringbuffer, input, inputLength);

  }

}