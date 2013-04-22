//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <stdlib.h>
#include <assert.h>
#include "Strobe.h"


Strobe* Strobe_create(int bufferLength, int resampledBufferLength, int samplerate, int samplesPerPeriod)
{
  Strobe* str = malloc(sizeof(Strobe));
  assert(str != NULL);

  str->samplerate            = samplerate;
  str->samplesPerPeriod      = samplesPerPeriod;
  str->bufferLength          = bufferLength;
  str->resampledBufferLength = resampledBufferLength;

  str->bandpass         = Biquad_create(3);
  str->src              = SRC_create(1, 1);
  str->filteredBuffer   = malloc(bufferLength * sizeof(double)); assert(str->filteredBuffer != NULL);
  str->resampledBuffer  = malloc(resampledBufferLength * sizeof(double));
                          assert(str->resampledBuffer != NULL);
  str->ringbufferData   = malloc(65536 * sizeof(double));
                          assert(str->ringbufferData != NULL);
  str->ringbuffer       = malloc(sizeof(PaUtilRingBuffer));
                          assert(str->ringbuffer != NULL);
  PaUtil_InitializeRingBuffer(str->ringbuffer, sizeof(double), 65536, str->ringbufferData);

  return str;
}

void Strobe_destroy(Strobe* str)
{
  Biquad_destroy(str->bandpass);
  SRC_destroy(str->src);
  free(str->filteredBuffer);
  free(str->resampledBuffer);
  free(str->ringbufferData);
  free(str->ringbuffer);
  free(str);
  str = NULL;
}

void Strobe_read(Strobe* str, float* output, int output_length)
{
  while (PaUtil_GetRingBufferReadAvailable(str->ringbuffer) >=  output_length)
    PaUtil_ReadRingBuffer(str->ringbuffer, output, output_length);
}

void Strobe_setFreq(Strobe* str, double freq)
{
  if (freq == str->freq)
    return;

  str->freq = freq;
  SRC_setRatio(str->src, freq * str->samplesPerPeriod, str->samplerate);
  SRC_reset(str->src);
  Biquad_reset(str->bandpass);
  Biquad_bandpass(str->bandpass, freq, str->samplerate, 10);
}

void Strobe_process(Strobe* str, float* input, int input_length)
{
  Biquad_filter(str->bandpass, input, input_length, str->filteredBuffer, input_length);
  int count = SRC_cubicConvert(str->src, str->filteredBuffer, input_length, str->resampledBuffer, str->resampledBufferLength);
  PaUtil_WriteRingBuffer(str->ringbuffer, str->resampledBuffer, count);
}