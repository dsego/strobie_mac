/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "kiss_fft.h"
#include "kiss_fftr.h"


// a pitch estimator based on FFT
typedef struct {

  float* timeData;

  kiss_fft_cpx* fftData;
  kiss_fftr_cfg fftCfg;

  int fftLength;         // FFT frame length
  int binCount;          // number of frequency bins
  int hopSize;           // FFT step
  double overlapFactor;  // overlap or oversampling factor
  double freqPerBin;     // bin frequency width
  double phaseStep;      // expected phase difference
  double samplerate;

} FFT_Pitch;


FFT_Pitch* FFT_Pitch_create(int samplerate, int fftLength, double overlapFactor);

void FFT_Pitch_destroy(FFT_Pitch* self);

double FFT_Pitch_estimateByInterpolation(FFT_Pitch* self, float* data, int dataLength);

double FFT_Pitch_estimateFromPhase(FFT_Pitch* self, float* data, int dataLength);
