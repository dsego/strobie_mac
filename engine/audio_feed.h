/*
  Copyright (C) 2013 Davorin Šego
*/

#include "portaudio.h"
#include "pa_ringbuffer.h"

typedef struct {
  PaUtilRingBuffer* ringbuffer;
  float* ringbuffer_data;
} AudioFeed;



AudioFeed* AudioFeed_create();
void AudioFeed_destroy();

// Read the newest data from the ring buffer
void AudioFeed_read(AudioFeed* af, float* output, int output_length);

// write into the ring buffer
void AudioFeed_process(AudioFeed* af, float* input, int input_length);