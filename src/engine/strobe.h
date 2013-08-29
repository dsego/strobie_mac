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
  int samplesPerPeriod;
  double freq;                  // strobing frequency

  FloatArray filteredBuffer;
  FloatArray resampledBuffer;
  FloatArray rbdata;

  Biquad* bandpass;             // band pass filter
  Interpolator* src;            // sample rate converter
  PaUtilRingBuffer* ringbuffer; // circular buffer

} Strobe;





Strobe* Strobe_create(int bufferLength, int resampledLength, int samplerate, int samplesPerPeriod );

void Strobe_destroy(Strobe* self);

// Read the newest data from the ring buffer
void Strobe_read(Strobe* self, float* output, int length);

// Set the filter band and the sample rate to a multiple of the target frequency
void Strobe_setFreq(Strobe* self, double freq);

// Process (IIR, re-sample) the audio input and write into the ring buffer
void Strobe_process(Strobe* self, float* input, int length);