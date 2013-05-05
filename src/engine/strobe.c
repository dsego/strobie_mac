/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "Strobe.h"

#define STROBE_RB_LENGTH 65536


Strobe* Strobe_create(
  int bufferLength,
  int resampledBufferLength,
  int samplerate,
  int samplesPerPeriod
) {

  Strobe* self = malloc(sizeof(Strobe));
  assert(self != NULL);

  self->samplerate            = samplerate;
  self->samplesPerPeriod      = samplesPerPeriod;
  self->bufferLength          = bufferLength;
  self->resampledBufferLength = resampledBufferLength;


  self->bandpass = Biquad_create(3);
  self->src      = Interpolator_create(1, 1);

  self->filteredBuffer = malloc(bufferLength * sizeof(float));
  assert(self->filteredBuffer != NULL);

  self->resampledBuffer = malloc(resampledBufferLength * sizeof(float));
  assert(self->resampledBuffer != NULL);

  self->ringbufferData = malloc(STROBE_RB_LENGTH * sizeof(float));
  assert(self->ringbufferData != NULL);

  self->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
  assert(self->ringbuffer != NULL);

  PaUtil_InitializeRingBuffer(
    self->ringbuffer,
    sizeof(float),
    STROBE_RB_LENGTH,
    self->ringbufferData
  );

  return self;

}


void Strobe_destroy(Strobe* self) {

  Biquad_destroy(self->bandpass);
  Interpolator_destroy(self->src);

  free(self->filteredBuffer);
  free(self->resampledBuffer);
  free(self->ringbufferData);
  free(self->ringbuffer);

  free(self);
  self = NULL;

}


void Strobe_read(Strobe* self, float* output, int output_length) {

  while (PaUtil_GetRingBufferReadAvailable(self->ringbuffer) >= output_length) {
    PaUtil_ReadRingBuffer(self->ringbuffer, output, output_length);
  }

}


void Strobe_setFreq(Strobe* self, double freq) {

  if (freq == self->freq) {
    return;
  }

  self->freq = freq;

  Interpolator_setRatio(
    self->src,
    freq * self->samplesPerPeriod,
    self->samplerate
  );

  Interpolator_reset(self->src);
  Biquad_reset(self->bandpass);
  Biquad_bandpass(self->bandpass, freq, self->samplerate, 10);

}


void Strobe_process(Strobe* self, float* input, int inputLength) {

  Biquad_filter(
    self->bandpass,
    input,
    self->filteredBuffer,
    inputLength
  );

  int count = Interpolator_cubicConvert(
    self->src,
    self->filteredBuffer,
    self->bufferLength,
    self->resampledBuffer,
    self->resampledBufferLength
  );

  if (count > 0) {
    PaUtil_WriteRingBuffer(self->ringbuffer, self->resampledBuffer, count);
  }

}