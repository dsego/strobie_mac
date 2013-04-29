/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "kiss_fft.h"
#include "kiss_fftr.h"


// a pitch estimator based on autocorrelation
typedef struct {

  float* paddedData;
  float* autocorrData;
  int fftLength;

  kiss_fftr_cfg fftCfg;
  kiss_fftr_cfg ifftCfg;

  kiss_fft_cpx* fft;
  int binCount;

  double samplerate;
  double fundamental;
  double fftPeak;

} PitchEstimation;


PitchEstimation* PitchEstimation_create(int sample_rate, int fftLength);

void PitchEstimation_destroy(PitchEstimation* self);

double PitchEstimation_fromAutocorrelation(
  PitchEstimation* self, float* data, int dataLength);

double PitchEstimation_fromFFT(PitchEstimation* self, float* data, int dataLength);