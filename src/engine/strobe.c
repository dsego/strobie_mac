/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "Strobe.h"

#define STROBE_RB_LENGTH 16384


Strobe* Strobe_create(
  int bufferLength,
  int resampledLength,
  int samplerate,
  int samplesPerPeriod
) {

  Strobe* self = malloc(sizeof(Strobe));
  assert(self != NULL);

  self->samplerate = samplerate;
  self->samplesPerPeriod = samplesPerPeriod;

  self->bandpass = Biquad_create(3);
  self->src = Interpolator_create(1, 1);

  self->filteredBuffer = FloatArray_create(bufferLength);
  self->resampledBuffer = FloatArray_create(resampledLength);
  self->rbdata = FloatArray_create(STROBE_RB_LENGTH);

  self->bufferRatio = resampledLength / bufferLength;

  self->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(self->ringbuffer != NULL);

  PaUtil_InitializeRingBuffer(
    self->ringbuffer,
    sizeof(float),
    STROBE_RB_LENGTH,
    self->rbdata.elements
  );

  return self;

}


void Strobe_destroy(Strobe* self) {

  Biquad_destroy(self->bandpass);
  Interpolator_destroy(self->src);
  FloatArray_destroy(self->filteredBuffer);
  FloatArray_destroy(self->resampledBuffer);
  FloatArray_destroy(self->rbdata);
  free(self->ringbuffer);
  free(self);
  self = NULL;

}


void Strobe_read(Strobe* self, float* output, int length) {

  // advance pointer to latest data
  while (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= 2 * length) {
    PaUtil_AdvanceRingBufferReadIndex(self->ringbuffer, length);
  }

  // read if there is data available
  while (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= length) {
    PaUtil_ReadRingBuffer(self->ringbuffer, output, length);
  }

}


void Strobe_setFreq(Strobe* self, float freq) {

  if (freq == self->freq) { return; }

  float newRate = freq * self->samplesPerPeriod;
  float ratio = newRate / self->samplerate;

  // ensure that the re-sampled data can fit into the buffer
  if (ratio > self->bufferRatio) {
    printf("Re-sampling buffer is too small");
    return;
  }

  self->freq = freq;
  Interpolator_setRatio(self->src, newRate, self->samplerate);
  Interpolator_reset(self->src);
  Biquad_reset(self->bandpass);
  Biquad_bandpass(self->bandpass, freq, self->samplerate, 10);

}


void Strobe_process(Strobe* self, float* input, int length) {

  Biquad_filter(self->bandpass, input, self->filteredBuffer.elements, length);

  // filteredBuffer.length might be larger than length!
  int count = Interpolator_cubicConvert(
    self->src,
    self->filteredBuffer.elements,
    length,
    self->resampledBuffer.elements,
    self->resampledBuffer.length
  );

  if (count > 0) {
    PaUtil_WriteRingBuffer(self->ringbuffer, self->resampledBuffer.elements, count);
  }

}


#undef STROBE_RB_LENGTH