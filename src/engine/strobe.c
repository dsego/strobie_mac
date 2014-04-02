// Copyright (c) Davorin Šego. All rights reserved.

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "Strobe.h"
#include "resonator.h"
#include "atomic.h"


#define STROBE_RB_LENGTH 16384
#define STROBE_FILTER_BW 12.0     // filter bandwidth in Hz
// #define STROBE_FILTER_Q 100


Strobe* Strobe_create(int bufferLength, int resampledLength, int samplesPerPeriod, int subdivCount) {

  Strobe* self = malloc(sizeof(Strobe));
  assert(self != NULL);

  self->samplesPerPeriod  = samplesPerPeriod;
  self->subdivCount       = subdivCount;
  self->samplerate        = -1;  // invalid value
  self->bandpass          = Biquad_create(3);
  self->src               = Intp_create(1);
  self->filteredBuffer    = Buffer_create(bufferLength, sizeof(float));
  self->resampledBuffer   = Buffer_create(resampledLength, sizeof(float));
  self->rbdata            = Buffer_create(STROBE_RB_LENGTH, sizeof(float));
  self->freqChanged = 0;

  self->bufferRatio = resampledLength / (float)bufferLength;
  self->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(self->ringbuffer != NULL);
  PaUtil_InitializeRingBuffer(
    self->ringbuffer,
    sizeof(float),
    STROBE_RB_LENGTH,
    self->rbdata->data
  );

  return self;

}


void Strobe_destroy(Strobe* self) {

  Biquad_destroy(self->bandpass);
  Intp_destroy(self->src);
  Buffer_destroy(self->filteredBuffer);
  Buffer_destroy(self->resampledBuffer);
  Buffer_destroy(self->rbdata);
  free(self->ringbuffer);
  free(self);
  self = NULL;

}


// returns 0 if no data was read
int Strobe_read(Strobe* self, float* output, int length) {

  // avoid an infinite loop
  if (length <= 0) return 0;

  // avoid displaying the filter transient
  if (self->freqChanged == 1) {
    int available = PaUtil_GetRingBufferReadAvailable(self->ringbuffer);
    PaUtil_AdvanceRingBufferReadIndex(self->ringbuffer, available);
    // CASB(self->freqChanged, 0, &self->freqChanged);
    FullMemoryBarrier();
    self->freqChanged = 0;
  }

  // advance pointer to latest data
  while (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= 2 * length) {
    PaUtil_AdvanceRingBufferReadIndex(self->ringbuffer, length);
  }

  // read latest data
  if (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= length) {
    PaUtil_ReadRingBuffer(self->ringbuffer, output, length);
    return 1;
  }

  return 0;

}


void Strobe_setFreq(Strobe* self, float freq, int samplerate) {

  if (freq < 1 || samplerate < 1) return;
  if (self->samplerate == samplerate && fabs(freq - self->freq) < 0.001) return;

  self->samplerate = samplerate;
  double newRate = freq * self->samplesPerPeriod;
  double ratio = newRate / (double)samplerate;

  // re-sampled data should fit into the buffer
  if (ratio > self->bufferRatio) {
    printf("Re-sampling buffer is too small \n");
    fflush(stdout);
    return;
  }

  self->freq = freq;

  FullMemoryBarrier();
  // CASB(self->freqChanged, 1, &self->freqChanged);
  self->freqChanged = 1;

  Intp_setRatio(self->src, newRate/samplerate);
  Intp_reset(self->src);

  double a0, a1, a2, b1, b2;
  resonator(freq/samplerate, STROBE_FILTER_BW/samplerate, &a0, &a1, &a2, &b1, &b2);
  Biquad_reset(self->bandpass);
  Biquad_setCoefficients(self->bandpass, a0, a1, a2, b1, b2);

}


void Strobe_process(Strobe* self, const float* input, int length) {

  Biquad_filter(self->bandpass, input, self->filteredBuffer->data, length);

  // filteredBuffer.length might be larger than length
  int count = Intp_linear(
    self->src,
    self->filteredBuffer->data,
    length,
    self->resampledBuffer->data,
    self->resampledBuffer->count
  );

  PaUtil_WriteRingBuffer(self->ringbuffer, self->resampledBuffer->data, count);

}



#undef STROBE_FILTER_BW
#undef STROBE_RB_LENGTH
