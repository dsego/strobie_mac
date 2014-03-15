// Copyright (c) Davorin Šego. All rights reserved.

#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "Biquad.h"
#include "Intp.h"
#include "Buffer.h"


// creates a strobe effect by re-sampling the signal
//  to a multiple of the note's frequency
typedef struct {

  int samplesPerPeriod;         // number of samples that describe one sound wave period
  float freq;                   // strobing frequency
  int subdivCount;
  int samplerate;
  volatile int freqChanged;

  Buffer* filteredBuffer;          // filtered data to be re-sampled
  Buffer* resampledBuffer;         // holds re-sampled data
  Buffer* rbdata;                  // circular buffer data store
  float bufferRatio;            // resampledBuffer.length / filteredBuffer.length

  Biquad* bandpass;             // band pass filter
  Intp* src;                    // sample rate converter
  PaUtilRingBuffer* ringbuffer; // circular buffer

} Strobe;




Strobe* Strobe_create(int bufferLength, int resampledLength, int samplesPerPeriod, int subdivCount);
void Strobe_destroy(Strobe* self);

// Read data from the ring buffer (return 0 if no data was read)
int Strobe_read(Strobe* self, float* output, int length);

// Set strobing frequency to a multiple of the target frequency
void Strobe_setFreq(Strobe* self, float freq, int samplerate);

// Process (IIR, re-sample) the audio input and write into the ring buffer
void Strobe_process(Strobe* self, const float* input, int length);
