/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */

#include <complex.h>
#include "Vec.h"
#include "ffts.h"


typedef struct {

  Vec* window;          // FFT windowing function
  Vec* audio;           // audio samples
  Vec* fft;             // frequency data
  Vec* spectrum;

  ffts_plan_t *fftPlan;       // FFTS plan
  ffts_plan_t *cepPlan;       // FFTS plan
  ffts_plan_t *ifftPlan;      // FFTS plan (inverse)

  float samplerate;

} Cepstrum;





Cepstrum* Cepstrum_create(int samplerate, int windowSize);
void Cepstrum_destroy(Cepstrum* self);
void Cepstrum_estimate(Cepstrum* self, const float* data, float *outFreq, float *outAmp);
