/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */

#include <complex.h>
#include "NSDF.h"
#include "Cepstrum.h"
// #include "HPS.h"



typedef enum { CEPSTRUM_METHOD, NSDF_METHOD, HPS_METHOD } EstimationMethod;


typedef struct {

  EstimationMethod method;
  void *estimator;

} Pitch;


Pitch* Pitch_create(EstimationMethod method, int samplerate, int windowSize);
void Pitch_destroy(Pitch* self);
void Pitch_estimate(Pitch* self, const float* data, float *outFreq, float *outAmp);
