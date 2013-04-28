/*
  Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "portaudio.h"
#include "pa_ringbuffer.h"


typedef struct {

  PaUtilRingBuffer* ringbuffer;
  float* ringbufferData;

} AudioFeed;



AudioFeed* AudioFeed_create();

void AudioFeed_destroy();

// Read the newest data from the ring buffer
void AudioFeed_read(AudioFeed* af, float* output, int outputLength);

// write into the ring buffer
void AudioFeed_process(AudioFeed* af, float* input, int inputLength);