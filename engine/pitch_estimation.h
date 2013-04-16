/*
  Copyright (C) 2013 Davorin Šego
*/

// #define kiss_fft_scalar double
#include "kiss_fftr.h"


typedef struct {
  float* padded_data;
  float* autocorr_data;
  int sample_rate;
  int fft_len;
  kiss_fft_cpx* fft;
  kiss_fftr_cfg fft_cfg;
  kiss_fftr_cfg ifft_cfg;
} PitchEstimation;


PitchEstimation* PitchEstimation_create(int sample_rate, int fft_length);
void PitchEstimation_destroy(PitchEstimation* pe);
double PitchEstimation_pitch_from_autocorrelation(PitchEstimation* pe, float* data, int data_len);