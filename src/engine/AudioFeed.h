/*
  Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "Buffer.h"



typedef struct {

  PaUtilRingBuffer* ringbuffer;
  Buffer* rbdata;

} AudioFeed;




AudioFeed* AudioFeed_create(void);
void AudioFeed_destroy(AudioFeed* self);

// Read the newest data from the ring buffer
void AudioFeed_read(AudioFeed* self, float* output, int outputLength);

// write into the ring buffer
void AudioFeed_process(AudioFeed* self, const float* input, int inputLength);
