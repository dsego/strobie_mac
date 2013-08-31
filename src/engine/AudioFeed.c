/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "AudioFeed.h"


#define RB_LENGTH 16384
#define FD_LENGTH 4096



AudioFeed* AudioFeed_create() {

  AudioFeed* self = malloc(sizeof(AudioFeed));
  assert(self != NULL);

  self->rbdata = FloatArray_create(RB_LENGTH);

  self->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(self->ringbuffer != NULL);

  PaUtil_InitializeRingBuffer(self->ringbuffer, sizeof(float), RB_LENGTH, self->rbdata.elements);

  self->filteredData = FloatArray_create(FD_LENGTH);
  self->decimatedData = FloatArray_create(FD_LENGTH);

  self->decimationCounter = 0;
  self->decimationRate = DECIMATE_NONE;

  self->halfbandIIR  = IIR_CreateHalfbandCheby();
  self->quartbandIIR = IIR_CreateQuartbandCheby();
  self->threePercIIR = IIR_CreateThreePercCheby();

  return self;

}


void AudioFeed_setDecimationRate(AudioFeed* self, DecimationRate decimationRate) {

  switch (decimationRate) {

    case DECIMATE_BY_TWO:
      self->activeIIR = self->halfbandIIR;
      self->decimationRate = DECIMATE_BY_TWO;
      IIR_reset(self->activeIIR);
      break;

    case DECIMATE_BY_FOUR:
      self->activeIIR = self->quartbandIIR;
      self->decimationRate = DECIMATE_BY_FOUR;
      IIR_reset(self->activeIIR);
      break;

    case DECIMATE_BY_THIRTY:
      self->activeIIR = self->threePercIIR;
      self->decimationRate = DECIMATE_BY_THIRTY;
      IIR_reset(self->activeIIR);
      break;

    case DECIMATE_NONE:
    default:
      self->activeIIR = NULL;
      self->decimationRate = DECIMATE_NONE;
      break;
  }

}


void AudioFeed_destroy(AudioFeed* self) {

  free(self->halfbandIIR);
  free(self->quartbandIIR);
  free(self->threePercIIR);
  FloatArray_destroy(self->rbdata);
  FloatArray_destroy(self->filteredData);
  FloatArray_destroy(self->decimatedData);
  free(self->ringbuffer);
  free(self);
  self = NULL;

}


void AudioFeed_read(AudioFeed* self, float* output, int length) {

  // advance pointer to latest data
  while (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= 2 * length) {
    PaUtil_AdvanceRingBufferReadIndex(self->ringbuffer, length);
  }

  // read if there is data available
  while (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= length) {
    PaUtil_ReadRingBuffer(self->ringbuffer, output, length);
  }

}


// 150Hz high pass Butterworth filter
inline static void highpass(float *input, float* output, int length) {

  const double a0 = 0.9850016172570234;
  const double a1 = -1.9700032345140468;
  const double a2 = 0.9850016172570234;
  const double b1 = -1.9697782746275025;
  const double b2 = 0.9702281944005912;

  static double z1 = 0.0;
  static double z2 = 0.0;

  for (int i = 0; i < length; ++i) {

    // Transposed direct form II
    output[i] = input[i] * a0 + z1;
    z1 = input[i] * a1 + z2 - b1 * output[i];
    z2 = input[i] * a2 - b2 * output[i];

  }

}


void AudioFeed_process(AudioFeed* self, float* input, int length) {

  // if (self->decimationRate != DECIMATE_NONE) {

  //   // anti-alias
  //   IIR_filter(self->activeIIR, input, self->filteredData, length);

  //   int n = self->decimationCounter;
  //   int k = 0;

  //   // sub sample
  //   while (n < length) {
  //     self->decimatedData[k] = self->filteredData[n];
  //     n += self->decimationRate;
  //     k += 1;
  //   }

  //   self->decimationCounter = n - length;

  //   PaUtil_WriteRingBuffer(self->ringbuffer, self->decimatedData, k);

  // }
  // else {

    // attenuate below 150Hz
    // highpass(input, self->filteredData, length);
    PaUtil_WriteRingBuffer(self->ringbuffer, input, length);
    // PaUtil_WriteRingBuffer(self->ringbuffer, self->filteredData, length);

  // }

}