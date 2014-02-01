/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */

#include <complex.h>
#include "Vec.h"
#include "ffts.h"


// Normalized square difference
typedef struct {

  Vec* audio;                 // audio samples
  Vec* nsdf;                  // normalized square difference
  Vec* fft;                   // FFT data
  // FloatArray top;          // SNAC numerator
  // FloatArray bottom;       // SNAC denominator
  ffts_plan_t *fftPlan;       // FFTS plan
  ffts_plan_t *ifftPlan;      // FFTS plan (inverse Fourier)

  float samplerate;

} NSDF;

NSDF* NSDF_create(int samplerate, int windowSize);
void NSDF_destroy(NSDF* self);
void NSDF_estimate(NSDF* self, const float* data, float *outFreq, float *outAmp);
