//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include "kiss_fft.h"
#include "kiss_fftr.h"


// a pitch estimator based on autocorrelation
typedef struct {
  float* paddedData;
  float* autocorrData;
  int samplerate;
  int fftLength;
  kiss_fft_cpx* fft;
  kiss_fftr_cfg fftCfg;
  kiss_fftr_cfg ifftCfg;
} PitchEstimation;


PitchEstimation* PitchEstimation_create(int sample_rate, int fftLength);
void PitchEstimation_destroy(PitchEstimation* pe);

double PitchEstimation_pitchFromAutocorrelation(PitchEstimation* pe, float* data, int dataLength);