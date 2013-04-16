/*
  Copyright (C) 2013 Davorin Šego
*/

#include <stdlib.h>
#include <assert.h>
#include "strobe.h"


Strobe* Strobe_create(int buffer_length, int resampled_buffer_length, int sample_rate, int samples_per_period)
{
  Strobe* str = malloc(sizeof(Strobe));
  assert(str != NULL);

  str->sample_rate             = sample_rate;
  str->samples_per_period      = samples_per_period;
  str->buffer_length           = buffer_length;
  str->resampled_buffer_length = resampled_buffer_length;

  str->bandpass           = Biquad_create(3);
  str->src                = SRC_create(1, 1);
  str->filtered_buffer    = malloc(buffer_length * sizeof(double)); assert(str->filtered_buffer != NULL);
  str->resampled_buffer   = malloc(resampled_buffer_length * sizeof(double));
                            assert(str->resampled_buffer != NULL);
  str->ringbuffer_data    = malloc(65536 * sizeof(double));
                            assert(str->ringbuffer_data != NULL);
  str->ringbuffer         = malloc(sizeof(PaUtilRingBuffer));
                            assert(str->ringbuffer != NULL);
  PaUtil_InitializeRingBuffer(str->ringbuffer, sizeof(double), 65536, str->ringbuffer_data);

  return str;
}

void Strobe_destroy(Strobe* str)
{
  Biquad_destroy(str->bandpass);
  SRC_destroy(str->src);
  free(str->filtered_buffer);
  free(str->resampled_buffer);
  free(str->ringbuffer_data);
  free(str->ringbuffer);
  free(str);
}

void Strobe_read(Strobe* str, float* output, int output_length)
{
  while (PaUtil_GetRingBufferReadAvailable(str->ringbuffer) >=  output_length)
    PaUtil_ReadRingBuffer(str->ringbuffer, output, output_length);
}

void Strobe_set_freq(Strobe* str, double freq)
{
  if (freq == str->freq)
    return;

  str->freq = freq;
  SRC_set_ratio(str->src, freq * str->samples_per_period, str->sample_rate);
  SRC_reset(str->src);
  Biquad_reset(str->bandpass);
  Biquad_bandpass(str->bandpass, freq, str->sample_rate, 10);
}

void Strobe_process(Strobe* str, float* input, int input_length)
{
  Biquad_filter(str->bandpass, input, input_length, str->filtered_buffer, input_length);
  int count = SRC_cubic_convert(str->src, str->filtered_buffer, input_length, str->resampled_buffer, str->resampled_buffer_length);
  PaUtil_WriteRingBuffer(str->ringbuffer, str->resampled_buffer, count);
}