/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "kiss_fftr.h"
#include <complex.h>
#include "ffts.h"


// a pitch estimator based on FFT
typedef struct {

  float* window;
  float* audio;               // audio samples
  float* acf;                 // autocorrelation
  float* sdf;                 // square difference
  int* primaryPeaks;          // primary peak positions in NSDF

  int fftBinCount;            // number of frequency bins
  int fftLength;              // FFT frame length

  float complex *fft;         // frequency data
  float* powSpectrum;         // frequency power spectrum

  float complex* cepstrum;    // cepstrum -> FFT of power spectrum
  float* powCepstrum;
  int cepLength;
  int cepBinCount;


  ffts_plan_t *fftPlan;       // FFTS plan
  ffts_plan_t *cepPlan;       // FFTS plan for cepstrum
  ffts_plan_t *ifftPlan;      // FFTS plan (inverse)

  double freqPerBin;          // bin frequency width
  double samplerate;

} Pitch;





Pitch* Pitch_create(int samplerate, int fftLength);
void Pitch_destroy(Pitch* self);
double Pitch_estimate(Pitch* self, float* data, int dataLength);