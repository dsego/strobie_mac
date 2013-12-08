/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "AudioFeed.h"


#define RB_LENGTH 16384


AudioFeed* AudioFeed_create() {

  AudioFeed* self = malloc(sizeof(AudioFeed));
  assert(self != NULL);

  self->rbdata = Vec_create(RB_LENGTH, sizeof(float));
  self->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(self->ringbuffer != NULL);

  PaUtil_InitializeRingBuffer(self->ringbuffer, sizeof(float), RB_LENGTH, self->rbdata.elements);

  return self;

}


void AudioFeed_destroy(AudioFeed* self) {

  Vec_destroy(self->rbdata);
  free(self->ringbuffer);
  free(self);
  self = NULL;

}


void AudioFeed_read(AudioFeed* self, float* output, int length) {

  // avoid an infinite loop
  if (length <= 0) return;

  // advance pointer to latest data
  while (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= 2 * length) {
    PaUtil_AdvanceRingBufferReadIndex(self->ringbuffer, length);
  }

  // read if there is data available
  if (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= length) {
    PaUtil_ReadRingBuffer(self->ringbuffer, output, length);
  }

}


// 150Hz high pass Butterworth filter
inline static void highpass(float *input, float* output, int length) {

  const float a0 =  0.9850016172570234;
  const float a1 = -1.9700032345140468;
  const float a2 =  0.9850016172570234;
  const float b1 = -1.9697782746275025;
  const float b2 =  0.9702281944005912;

  static float z1 = 0.0;
  static float z2 = 0.0;

  for (int i = 0; i < length; ++i) {

    // Transposed direct form II
    output[i] = input[i] * a0 + z1;
    z1 = input[i] * a1 + z2 - b1 * output[i];
    z2 = input[i] * a2 - b2 * output[i];

  }

}


// moving average
// void smooth(float *input, float *output, int length) {

//   const int n = 16;
//   const float k = 1.0 / (float)n;
//   static float samples[n] = { 0.0 };
//   static float mean = 0.0;

//   for (int i = 0; i < n; ++i) {
//     mean = mean + (input[i] - samples[i]) * k;
//     output[i] = mean;
//   }

//   for (int i = n; i < length; ++i) {
//     mean = mean + (input[i] - input[i -n]) * k;
//     output[i] = mean;
//   }

//   for (int i = length - n, j=0; i < length; ++i, ++j) {
//     samples[j] = input[i];
//   }

// }


void AudioFeed_process(AudioFeed* self, float* input, int length) {

  // const int n = 1024;
  // static float smoothed[n];
  // int len = length;
  // int index = 0;

  // while (len > n) {
  //   smooth(&input[index], smoothed, n);
  //   PaUtil_WriteRingBuffer(self->ringbuffer, smoothed, n);
  //   len -= n;
  //   index += n;
  // }

  // if (len > 0) {
  //   smooth(&input[index], smoothed, len);
  //   PaUtil_WriteRingBuffer(self->ringbuffer, smoothed, len);
  // }

  // highpass(input, input, length);
  PaUtil_WriteRingBuffer(self->ringbuffer, input, length);

}


#undef RB_LENGTH

