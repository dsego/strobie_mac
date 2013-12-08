/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <complex.h>
#include "Array.h"
#include "ffts.h"


typedef struct {

  FloatArray window;          // FFT windowing function
  FloatArray audio;           // audio samples
  CpxFloatArray fft;          // frequency data
  FloatArray spectrum;

  ffts_plan_t *fftPlan;       // FFTS plan
  ffts_plan_t *cepPlan;       // FFTS plan
  ffts_plan_t *ifftPlan;      // FFTS plan (inverse)

  float samplerate;

} Cepstrum;





Cepstrum* Cepstrum_create(int samplerate, int windowSize);
void Cepstrum_destroy(Cepstrum* self);
void Cepstrum_estimate(Cepstrum* self, float* data, float *outFreq, float *outAmp);
