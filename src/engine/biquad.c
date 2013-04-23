//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "Biquad.h"

Biquad* Biquad_create(int sectionCount)
{
  assert(sectionCount < MAX_SECTION_COUNT);
  Biquad* bq = malloc(sizeof(Biquad));
  assert(bq != NULL);
  return bq;
}

void Biquad_destroy(Biquad* bq)
{
  free(bq);
  bq = NULL;
}

void Biquad_lowpass(Biquad* bq, double cutoff, double samplerate, double Q)
{
  double K    = tan(M_PI * cutoff / samplerate);
  double norm = 1 / (1 + K / Q + K * K);
  bq->a0      = K * K * norm;
  bq->a1      = 2 * bq->a0;
  bq->a2      = bq->a0;
  bq->b1      = 2 * (K * K - 1) * norm;
  bq->b2      = (1 - K / Q + K * K) * norm;
}

void Biquad_highpass(Biquad* bq, double cutoff, double samplerate, double Q)
{
  double K    = tan(M_PI * cutoff / samplerate);
  double norm = 1 / (1 + K / Q + K * K);
  bq->a0      = 1 * norm;
  bq->a1      = -2 * bq->a0;
  bq->a2      = bq->a0;
  bq->b1      = 2 * (K * K - 1) * norm;
  bq->b2      = (1 - K / Q + K * K) * norm;
}

void Biquad_bandpass(Biquad* bq, double cutoff, double samplerate, double Q)
{
  double K    = tan(M_PI * cutoff / samplerate);
  double norm = 1 / (1 + K / Q + K * K);
  bq->a0      = K / Q * norm;
  bq->a1      = 0;
  bq->a2      = -bq->a0;
  bq->b1      = 2 * (K * K - 1) * norm;
  bq->b2      = (1 - K / Q + K * K) * norm;
}

void Biquad_reset(Biquad* bq)
{
  for (int i = 0; i < bq->sectionCount; ++i) {
    bq->z1[i] = 0.0;
    bq->z2[i] = 0.0;
  }
}

void Biquad_filter(Biquad* bq, float* input, int inputLength, float* output, int outputLength)
{
  double y = 0.0;
  double x = 0.0;

  assert(inputLength == outputLength);

  for (int i = 0; i < inputLength; ++i) {
    x = (double) input[i];

    // Cascade into multiple bi-quad sections or stages
    for (int section = 0; section < bq->sectionCount; ++section) {
      // Transposed direct form II
      //   z2   = a2 * x[n-2] – b2 * y[n-2]
      //   z1   = a1 * x[n-1] – b1 * y[n-1] + z2
      //   y[n] = a0 * input[i]
      y               = x * bq->a0 + bq->z1[section];
      bq->z1[section] = x * bq->a1 + bq->z2[section] - bq->b1 * y;
      bq->z2[section] = x * bq->a2 - bq->b2 * y;
      x               = y;
    }
    output[i] = (float) y;
  }
}