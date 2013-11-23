/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "Biquad.h"
#include "Interpolator.h"
#include "Array.h"


// creates a strobe effect by re-sampling the signal
//  to a multiple of the note's frequency
typedef struct {

  int samplerate;
  int samplesPerPeriod;         // number of samples used to describe one sound wave period
  float freq;                   // strobing frequency
  int subdivCount;

  FloatArray filteredBuffer;    // store filtered data to be re-sampled
  FloatArray resampledBuffer;   // stores re-sampled data before writing to the circular buffer
  FloatArray rbdata;            // circular buffer data store
  float bufferRatio;            // resampledBuffer.length / filteredBuffer.length

  Biquad* bandpass;             // band pass filter
  Interpolator* src;            // sample rate converter
  PaUtilRingBuffer* ringbuffer; // circular buffer

} Strobe;




Strobe* Strobe_create(
  int bufferLength,
  int resampledLength,
  int samplerate,
  int samplesPerPeriod,
  int subdivCount
);

void Strobe_destroy(Strobe* self);

// Read the newest data from the ring buffer (return 0 if no data was read)
int Strobe_read(Strobe* self, float* output, int length);

// Set the filter band and the sample rate to a multiple of the target frequency
void Strobe_setFreq(Strobe* self, float freq);

// Process (IIR, re-sample) the audio input and write into the ring buffer
void Strobe_process(Strobe* self, float* input, int length);
