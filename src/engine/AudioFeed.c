//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <stdlib.h>
#include <assert.h>
#include "AudioFeed.h"


AudioFeed* AudioFeed_create()
{
  AudioFeed* af = malloc(sizeof(AudioFeed));
  assert(af != NULL);

  af->ringbufferData = malloc(32768 * sizeof(float));
  assert(af->ringbufferData != NULL);

  af->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(af->ringbuffer != NULL);

  PaUtil_InitializeRingBuffer(af->ringbuffer, sizeof(float), 32768, af->ringbufferData);
  return af;
}

void AudioFeed_destroy(AudioFeed* af)
{
  free(af->ringbufferData);
  free(af->ringbuffer);
  free(af);
  af = NULL;
}

void AudioFeed_read(AudioFeed* af, float* output, int outputLength)
{
  while (PaUtil_GetRingBufferReadAvailable(af->ringbuffer) >= outputLength)
    PaUtil_ReadRingBuffer(af->ringbuffer, output, outputLength);
}

void AudioFeed_process(AudioFeed* af, float* input, int inputLength)
{
  PaUtil_WriteRingBuffer(af->ringbuffer, input, inputLength);
}