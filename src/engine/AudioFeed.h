/*
  Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "Array.h"
#include "IIR.h"



typedef enum {

  DECIMATE_NONE      = 1,
  DECIMATE_BY_TWO    = 2,
  DECIMATE_BY_FOUR   = 4,
  DECIMATE_BY_THIRTY = 30

} DecimationRate;




typedef struct {

  PaUtilRingBuffer* ringbuffer;
  FloatArray rbdata;
  FloatArray filteredData;
  FloatArray decimatedData;
  int decimationCounter;

  IIR* activeIIR;       // points to one of the three filters below
  IIR* halfbandIIR;
  IIR* quartbandIIR;
  IIR* threePercIIR;

  DecimationRate decimationRate;

} AudioFeed;




AudioFeed* AudioFeed_create();
void AudioFeed_destroy();

// Read the newest data from the ring buffer
void AudioFeed_read(AudioFeed* self, float* output, int outputLength);

// decimate audio
void AudioFeed_setDecimationRate(AudioFeed* self, DecimationRate decimationRate);

// write into the ring buffer
void AudioFeed_process(AudioFeed* self, float* input, int inputLength);