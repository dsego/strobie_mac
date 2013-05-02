/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include "kiss_fft.h"
#include "kiss_fftr.h"


// a pitch estimator based on autocorrelation
typedef struct {

  float* paddedData;
  float* autocorrData;
  kiss_fft_cpx* fftData;

  kiss_fftr_cfg fftCfg;
  kiss_fftr_cfg ifftCfg;

  int binCount;
  int fftLength;
  double samplerate;

} AutocorrPitch;


AutocorrPitch* AutocorrPitch_create(int samplerate, int fftLength);

void AutocorrPitch_destroy(AutocorrPitch* self);

double AutocorrPitch_estimate(
  AutocorrPitch* self, float* data, int dataLength);