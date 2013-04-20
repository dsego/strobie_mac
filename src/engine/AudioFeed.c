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

  af->ringbuffer_data = malloc(32768 * sizeof(float));
  assert(af->ringbuffer_data != NULL);

  af->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(af->ringbuffer != NULL);

  PaUtil_InitializeRingBuffer(af->ringbuffer, sizeof(float), 32768, af->ringbuffer_data);
  return af;
}

void AudioFeed_destroy(AudioFeed* af)
{
  free(af->ringbuffer_data);
  free(af->ringbuffer);
  free(af);
  af = NULL;
}

void AudioFeed_read(AudioFeed* af, float* output, int output_length)
{
  while (PaUtil_GetRingBufferReadAvailable(af->ringbuffer) >= output_length)
    PaUtil_ReadRingBuffer(af->ringbuffer, output, output_length);
}

void AudioFeed_process(AudioFeed* af, float* input, int input_length)
{
  PaUtil_WriteRingBuffer(af->ringbuffer, input, input_length);
}