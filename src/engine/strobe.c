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
#define STROBE_FILTER_BW 8     // filter bandwidth in Hz
// #define STROBE_FILTER_Q 100


Strobe* Strobe_create(
  int bufferLength,
  int resampledLength,
  int samplerate,
  int samplesPerPeriod,
  int subdivCount) {

  Strobe* self = malloc(sizeof(Strobe));
  assert(self != NULL);

  self->samplerate = samplerate;
  self->samplesPerPeriod = samplesPerPeriod;
  self->subdivCount = subdivCount;

  self->bandpass = Biquad_create(2);
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

    // double the number of periods
    if (self->subdivCount == 1) {
      float peak = findWavePeak(output, length);
      float factor = 2.0 / peak;

      for (int i = 1; i < length; ++i) {
        output[i] = factor * output[i] * output[i] - peak;
      }
    }
    else if (self->subdivCount == 2) {
      float peak = findWavePeak(output, length);
      float factor = 2.0 / peak;

      for (int i = 1; i < length; ++i) {
        output[i] = factor * output[i] * output[i] - peak;
        output[i] = factor * output[i] * output[i] - peak;
      }
    }

    return 1;

  }
  else {

    return 0;

  }

}


void Strobe_setFreq(Strobe* self, float freq) {

  // assume freq is positive
  if (fabs(freq - self->freq) < 0.000001) { return; }

  float newRate = freq * self->samplesPerPeriod;
  float ratio = newRate / self->samplerate;

  // re-sampled data should fit into the buffer
  if (ratio > self->bufferRatio) {
    printf("Re-sampling buffer is too small \n");
    fflush(stdout);
    return;
  }

  self->freq = freq;

  // skip stale data
  int available = PaUtil_GetRingBufferReadAvailable(self->ringbuffer);
  PaUtil_AdvanceRingBufferReadIndex(self->ringbuffer, available);

  Interpolator_setRatio(self->src, newRate, self->samplerate);
  Interpolator_reset(self->src);
  Biquad_reset(self->bandpass);

  double a[3], b[3];
  // double bw = freq * exp2(25.0/1200.0) - freq;
  // printf("%f %f \n", freq, bw); fflush(stdout);
  // resonator(freq, bw, self->samplerate, a, b);
  resonator(freq, STROBE_FILTER_BW, self->samplerate, a, b);
  Biquad_setCoefficients(self->bandpass, a[0], a[1], a[2], b[1], b[2]);
  // Biquad_bandpass(self->bandpass, freq, self->samplerate, STROBE_FILTER_Q);

}


static inline void _process(Strobe* self, float* input, int length) {

  Biquad_filter(self->bandpass, input, self->filteredBuffer.elements, length);

  // filteredBuffer.length might be larger than length!
  int count = Interpolator_cubicConvert(
    self->src,
    self->filteredBuffer.elements,
    length,
    self->resampledBuffer.elements,
    self->resampledBuffer.length
  );

  PaUtil_WriteRingBuffer(self->ringbuffer, self->resampledBuffer.elements, count);

}


void Strobe_process(Strobe* self, float* input, int length) {

  int index = 0;
  int inlen = length;
  int bufflen = self->filteredBuffer.length;

  // break into digestible chunks
  while (inlen > bufflen) {
    _process(self, &input[index], bufflen);
    index += bufflen;
    inlen -= bufflen;
  };

  if (inlen > 0) {
     _process(self, &input[index], inlen);
  }

}





#undef STROBE_RB_LENGTH
