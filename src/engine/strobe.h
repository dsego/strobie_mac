/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "Biquad.h"
#include "Interpolator.h"


// creates a strobe effect by re-sampling the signal to a multiple of the note's frequency
typedef struct {

  int samplerate;
  int samplesPerPeriod;
  int bufferLength;
  int resampledBufferLength;

  // strobing frequency
  double freq;

  // buffers for processing data before writing to the ring buffer
  float* filteredBuffer;
  float* resampledBuffer;

  // band pass filter
  Biquad*  bandpass;

  // sample rate converter
  Interpolator* src;

  // circular buffer
  PaUtilRingBuffer* ringbuffer;
  float* ringbufferData;

} Strobe;



Strobe* Strobe_create(int bufferLength, int resampledBufferLength, int samplerate, int samplesPerPeriod);

void Strobe_destroy(Strobe* str);

// Read the newest data from the ring buffer
void Strobe_read(Strobe* str, float* output, int outputLength);

// Set the filter band and the sample rate to a multiple of the target frequency
void Strobe_setFreq(Strobe* str, double freq);

// Process (IIR, re-sample) the audio input and write into the ring buffer
void Strobe_process(Strobe* str, float* input, int inputLength);