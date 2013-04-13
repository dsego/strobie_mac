/*
  Copyright (C) 2013 Davorin Šego
*/
#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "biquad.h"
#include "src.h"

typedef struct {
  int sample_rate;
  int samples_per_period;
  int buffer_length;
  int resampled_buffer_length;

  // strobing frequency
  double freq;

  // buffers for processing data before writing to the ring buffer
  double* filtered_buffer;
  double* resampled_buffer;

  // band pass filter
  Biquad*  bandpass;

  // sample rate converter
  SRC* src;

  // circular buffer
  PaUtilRingBuffer* ringbuffer;
  double* ringbuffer_data;

} Strobe;



Strobe* Strobe_create(int buffer_length, int resampled_buffer_length, int sample_rate, int samples_per_period);
void Strobe_destroy(Strobe* str);

// Read the newest data from the ring buffer
void Strobe_read(Strobe* str, double* output, int output_length);

// Set the filter band and the sample rate to a multiple of the target frequency
void Strobe_set_freq(Strobe* str, double freq);

// Process (IIR, re-sample) the audio input and write into the ring buffer
void Strobe_process(Strobe* str, double* input, int input_length);