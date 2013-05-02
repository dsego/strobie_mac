/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "FFT_Pitch.h"
#include "kiss_fftr.h"
#include "windows.h"


FFT_Pitch* FFT_Pitch_create(int samplerate, int fftLength, double overlapFactor) {

  FFT_Pitch* self = malloc(sizeof(FFT_Pitch));
  assert(self != NULL);

  self->samplerate = samplerate;
  self->fftLength = fftLength;
  self->overlapFactor = overlapFactor;

  self->hopSize = (int) ceil((double)fftLength / overlapFactor);

  // expected phase difference for this hop size
  self->phaseStep = 2.0 * M_PI * self->hopSize / fftLength;

  self->freqPerBin = samplerate / (double)fftLength;
  self->binCount   = fftLength / 2 + 1;

  self->fftCfg  = kiss_fftr_alloc(fftLength, 0, NULL, NULL);


  self->timeData = calloc(self->fftLength, sizeof(float));
  assert(self->timeData != NULL);

  self->fftData = malloc(self->binCount * sizeof(kiss_fft_cpx));
  assert(self->fftData != NULL);

  return self;

}


void FFT_Pitch_destroy(FFT_Pitch* self) {

  kiss_fftr_free(self->fftCfg);
  free(self->fftData);
  free(self);
  self = NULL;

}


// estimate "true" frequency based on phase change between FFT frames
double FFT_Pitch_trueFreq(FFT_Pitch* self, double binIndex, double phase, double lastPhase) {

  // measured phase difference
  double phaseDiff = phase - lastPhase;

  // how much the phase difference diverges from the expected difference
  double delta = phaseDiff - binIndex * self->phaseStep;

  // wrap delta phase into +/- PI interval
  delta = remainder(delta, 2.0 * M_PI);

  // relative  difference
  delta /= self->phaseStep;

  // estimated "true" frequency
  return (binIndex + delta) * self->freqPerBin;

}

// Quadratic Interpolation of Spectral Peaks
//   https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html */
double FFT_Pitch_parabolaPeakLocation(double y0, double y1, double y2) {

  return 0.5 * (y0 - y2) / (y0 - 2.0 * y1 + y2);

}


double FFT_Pitch_estimateByInterpolation(FFT_Pitch* self, float* data, int dataLength) {

  // data contains two overlapped frames
  memcpy(self->timeData, data, self->fftLength * sizeof(float));

  // First Fourier transform
  applyBlackmanHarris(self->timeData, self->fftLength);
  kiss_fftr(self->fftCfg, self->timeData, self->fftData);

  double strongest = 0;
  int bin = 0;
  double norm = 1.0 / self->fftLength;

  // find the strongest frequency bin
  for (int i = 0; i < self->binCount - 1; ++i) {

    self->fftData[i].r = self->fftData[i].r * self->fftData[i].r +
      self->fftData[i].i * self->fftData[i].i;

    self->fftData[i].r *= norm;

    if (self->fftData[i].r > strongest) {
      strongest = self->fftData[i].r;
      bin = i;
    }

  }

  // estimate true pitch by quadratic interpolation
  double trueFreq = bin + FFT_Pitch_parabolaPeakLocation(
    log(sqrt(self->fftData[bin-1].r)),
    log(sqrt(self->fftData[bin].r)),
    log(sqrt(self->fftData[bin+1].r))
  );

  return trueFreq * self->freqPerBin;

}


double FFT_Pitch_estimateFromPhase(FFT_Pitch* self, float* data, int dataLength) {

  // data contains two overlapped frames

  memcpy(self->timeData, data, self->fftLength * sizeof(float));

  // First Fourier transform
  applyHann(self->timeData, self->fftLength);
  kiss_fftr(self->fftCfg, self->timeData, self->fftData);

  double magnitude = 0;
  double strongest = 0;
  double phase = 0;
  int bin = 0;

  double norm = 1.0 / self->fftLength;

  // find the strongest frequency bin
  for (int i = 0; i < self->binCount - 1; ++i) {

    magnitude = norm * (self->fftData[i].r * self->fftData[i].r +
      self->fftData[i].i * self->fftData[i].i);

    if (magnitude > strongest) {
      bin = i;
      strongest = magnitude;
    }

  }

  // arctan( Im / Re )
  phase = atan2(self->fftData[bin].i, self->fftData[bin].r);

  memcpy(self->timeData, &data[self->hopSize], self->fftLength * sizeof(float));

  // Second Fourier transform
  applyHann(self->timeData, self->fftLength);
  kiss_fftr(self->fftCfg, self->timeData, self->fftData);

  double newPhase = atan2(self->fftData[bin].i, self->fftData[bin].r);

  // estimate "true" frequency
  double trueFreq = FFT_Pitch_trueFreq(self, bin, newPhase, phase);

  return trueFreq;

}