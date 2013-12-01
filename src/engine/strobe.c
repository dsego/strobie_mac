/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "Strobe.h"
#include "utils.h"
#include "resonator.h"


#define STROBE_RB_LENGTH 16384
#define STROBE_FILTER_BW 12     // filter bandwidth in Hz
// #define STROBE_FILTER_Q 100


Strobe* Strobe_create(
  int bufferLength,
  int resampledLength,
  int samplesPerPeriod,
  int subdivCount) {

  Strobe* self = malloc(sizeof(Strobe));
  assert(self != NULL);

  self->samplesPerPeriod = samplesPerPeriod;
  self->subdivCount = subdivCount;

  // invalid value
  self->samplerate = -1;

  self->bandpass = Biquad_create(3);
  self->src = Interpolator_create(1, 1);

  self->filteredBuffer = FloatArray_create(bufferLength);
  self->resampledBuffer = FloatArray_create(resampledLength);
  self->rbdata = FloatArray_create(STROBE_RB_LENGTH);

  self->bufferRatio = resampledLength / (float)bufferLength;

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


// returns 0 if no data was read
int Strobe_read(Strobe* self, float* output, int length) {

  // avoid an infinite loop
  if (length <= 0) return 0;

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

  if (freq < 1.0 || samplerate < 1.0) return;
  if (self->samplerate == samplerate && fabs(freq - self->freq) < 0.000001) return;

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
  // skip stale data
  int available = PaUtil_GetRingBufferReadAvailable(self->ringbuffer);
  // PaUtil_AdvanceRingBufferReadIndex(self->ringbuffer, available);

  Interpolator_setRatio(self->src, newRate, samplerate);
  Interpolator_reset(self->src);
  Biquad_reset(self->bandpass);

  double a[3], b[3];
  // double bw = freq * exp2(25.0/1200.0) - freq;
  // resonator(freq, bw, samplerate, a, b);
  resonator(freq, STROBE_FILTER_BW, samplerate, a, b);
  Biquad_setCoefficients(self->bandpass, a[0], a[1], a[2], b[1], b[2]);
  // Biquad_bandpass(self->bandpass, freq, samplerate, STROBE_FILTER_Q);

}


void Strobe_process(Strobe* self, float* input, int length) {

  Biquad_filter(self->bandpass, input, self->filteredBuffer.elements, length);

  // filteredBuffer.length might be larger than length
  int count = Interpolator_cubicConvert(
    self->src,
    self->filteredBuffer.elements,
    length,
    self->resampledBuffer.elements,
    self->resampledBuffer.length
  );

  PaUtil_WriteRingBuffer(self->ringbuffer, self->resampledBuffer.elements, count);

}



#undef STROBE_FILTER_BW
#undef STROBE_RB_LENGTH
