/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <complex.h>
#include "Array.h"
#include "ffts.h"


// a pitch estimator based on FFT
typedef struct {

  FloatArray window;          // FFT window
  FloatArray audio;           // audio samples
  FloatArray nsdf;            // normalized square difference
  // FloatArray top;             // SNAC numerator
  // FloatArray bottom;          // SNAC denominator

  CpxFloatArray fft;          // frequency data
  CpxFloatArray cepstrum;     // cepstrum -> FFT of power spectrum
  FloatArray powSpectrum;     // frequency power spectrum
  FloatArray powCepstrum;

  ffts_plan_t *fftPlan;       // FFTS plan
  ffts_plan_t *cepPlan;       // FFTS plan for cepstrum
  ffts_plan_t *ifftPlan;      // FFTS plan (inverse)

  float samplerate;

} Pitch;





Pitch* Pitch_create(int samplerate, int windowSize);
void Pitch_destroy(Pitch* self);
void Pitch_estimate(Pitch* self, float* data, float *outFreq, float *outAmp);
