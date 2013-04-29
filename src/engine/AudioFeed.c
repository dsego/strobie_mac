/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "AudioFeed.h"




AudioFeed* AudioFeed_create() {

  AudioFeed* self = malloc(sizeof(AudioFeed));
  assert(self != NULL);

  self->ringbufferData = malloc(32768 * sizeof(float));
  assert(self->ringbufferData != NULL);

  self->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(self->ringbuffer != NULL);

  PaUtil_InitializeRingBuffer(
    self->ringbuffer,
    sizeof(float),
    32768,
    self->ringbufferData
  );

  return self;

}


void AudioFeed_destroy(AudioFeed* self) {

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

  PaUtil_WriteRingBuffer(self->ringbuffer, input, inputLength);

}