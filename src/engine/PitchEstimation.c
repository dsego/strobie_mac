//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "PitchEstimation.h"
#include "kiss_fftr.h"

PitchEstimation* PitchEstimation_create(int samplerate, int fftLength)
{
  PitchEstimation* pe = malloc(sizeof(PitchEstimation));
                        assert(pe != NULL);

  pe->samplerate      = samplerate;
  pe->fftLength       = fftLength;

  pe->fftCfg          = kiss_fftr_alloc(fftLength, 0, NULL, NULL);
  pe->ifftCfg         = kiss_fftr_alloc(fftLength, 1, NULL, NULL);

  pe->fft             = malloc(sizeof(kiss_fft_cpx) * fftLength / 2 + 1);
                        assert(pe->fft != NULL);

  pe->paddedData      = calloc(sizeof(float),  fftLength * 2);
                        assert(pe->paddedData != NULL);

  pe->autocorrData    = malloc(sizeof(float) * fftLength);
                        assert(pe->autocorrData != NULL);
  return pe;
}

void PitchEstimation_destroy(PitchEstimation* pe)
{
  kiss_fftr_free(pe->fftCfg);
  kiss_fftr_free(pe->ifftCfg);
  free(pe->fft);
  free(pe->paddedData);
  free(pe->autocorrData);
  free(pe);
  pe = NULL;
}

void PitchEstimation_normalizeAndCenterClip(float* data, int dataLength)
{
  double max = 0;

  // find absolute max
  for (int i = 0; i < dataLength; ++i) {
    if (data[i] > max) {
      max = data[i];
    }
    else if (data[i] < -max) {
      max = -data[i];
    }
  }

  // normalization coefficient
  double k = 1.0 / max;

  // normalize & center clip
  for (int i = 0; i < dataLength; ++i) {
    data[i] *= k;
    if (-0.5 < data[i] && data[i] < 0.5) {
      data[i] = 0;
    }
  }
}

// Quadratic Interpolation of Spectral Peaks
//   https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html */
double PitchEstimation_parabolaPeakLocation(double y0, double y1, double y2)
{
  return 0.5 * (y0 - y2) / (y0 - 2.0 * y1 + y2);
}



double PitchEstimation_pitchFromAutocorrelation(PitchEstimation* pe, float* data, int dataLength)
{
  // important to get more consistent and accurate results!
  PitchEstimation_normalizeAndCenterClip(data, dataLength);

  // pad with zeros
  memcpy(pe->paddedData, data, dataLength * sizeof(float));

  // Fourier transform
  kiss_fftr(pe->fftCfg, pe->paddedData, pe->fft);

  // power spectrum  (a + bi)(a - bi) = (aa + bb) + (ab - ab)i
  for (int i = 0; i < pe->fftLength; ++i) {
    pe->fft[i].r = pe->fft[i].r * pe->fft[i].r + pe->fft[i].i * pe->fft[i].i;
    pe->fft[i].i = 0;
  }

  // inverse Fourier
  kiss_fftri(pe->ifftCfg, pe->fft, pe->autocorrData);

  // ramp
  for (int i = 1; i < pe->fftLength / 2; ++i) {
    pe->autocorrData[i] *= 1.0 + (pe->fftLength - i) / pe->fftLength;
  }

  int low = 0, x = 0;

  // find lowest point
  for (int i = 2; i < pe->fftLength / 2; ++i) {
    if (pe->autocorrData[i] > pe->autocorrData[i - 1]) {
      low = i - 1;
      break;
    }
  }

  // find peak (period)
  double threshold = 0.7 * pe->autocorrData[0];

  for (int i = low; i < pe->fftLength / 2; ++i) {
    if (pe->autocorrData[i] > threshold) {
      threshold = pe->autocorrData[i];
      x = i;
    }
  }

  double pitch = 0;

  // estimate true peak via parabolic interpolation
  double peakLocation = x + PitchEstimation_parabolaPeakLocation(pe->autocorrData[x - 1], pe->autocorrData[x], pe->autocorrData[x + 1]);

  if (peakLocation != 0) {
    pitch = (double) pe->samplerate / peakLocation;
  }

  return pitch;
}