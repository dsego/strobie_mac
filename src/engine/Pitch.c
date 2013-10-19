/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "Pitch.h"
#include "utils.h"
#include "ffts.h"


#define PI  3.14159265358979323846264338327950288419716939937510582097494459230




inline static void blackman(float* response, int length) {

  const int last = length - 1;
  const float factor = 2.0 * PI / last;
  const float alpha = 0.42;
  const float beta = 0.5;
  const float gamma = 0.08;

  for (int i = 0; i <= last; ++i) {
    response[i] = alpha - beta * cos(i * factor) + gamma * cos(2 * i * factor);
  }

}



/*

  http://www.embedded.com/design/embedded/4008835/DSP-Tricks-Frequency-Domain-Windowing

  -------------------------------------------------------
   window             alpha       beta        gamma
  -------------------------------------------------------
  rectangular         1.0         -           -
  hann                0.5         0.5         -
  hamming             0.54        0.46        -
  blackman            0.42        0.5         0.08
  exact blackman      7938/18608  9240/18608  1430/18608
  blackman-harris     0.42323     0.49755     0.07922
  -------------------------------------------------------

 */

// frequency domain five-term blackman window
inline static float complex blackmanFreq(
  const float complex a,
  const float complex b,
  const float complex c,
  const float complex d,
  const float complex e) {

  const float alpha = 0.42;
  const float beta  = 0.5;
  const float gamma = 0.08;
  return alpha * c + 0.5 * (gamma * (a + e) - beta * (b + d));

}



Pitch* Pitch_create(int samplerate, int fftLength) {

  Pitch* self = malloc(sizeof(Pitch));
  assert(self != NULL);

  self->samplerate = samplerate;

  int fftBinCount = fftLength; // + 1 ?

  // zero-pad (autocorrelation)
  fftLength = fftLength + fftLength;

  self->fftPlan = ffts_init_1d_real(fftLength, -1);
  self->ifftPlan = ffts_init_1d_real(fftLength, 1);

  self->fft = CpxFloatArray_create(fftBinCount);
  self->window = FloatArray_create(fftLength);
  self->audio = FloatArray_create(fftLength);
  self->acf = FloatArray_create(fftLength);
  self->sdf = FloatArray_create(fftLength);
  self->powSpectrum = FloatArray_create(fftBinCount);
  self->primaryPeaks = IntArray_create(fftLength);

  blackman(self->window.elements, fftLength);

  // Cepstrum
  int cepLength = fftLength / 2;
  int cepBinCount = cepLength / 2;
  self->cepPlan = ffts_init_1d_real(cepLength, -1);
  self->cepstrum = CpxFloatArray_create(cepBinCount);
  self->powCepstrum = FloatArray_create(cepBinCount);


  return self;

}


void Pitch_destroy(Pitch* self) {

  FloatArray_destroy(self->window);
  FloatArray_destroy(self->audio);
  FloatArray_destroy(self->acf);
  FloatArray_destroy(self->sdf);
  IntArray_destroy(self->primaryPeaks);
  FloatArray_destroy(self->powSpectrum);
  FloatArray_destroy(self->powCepstrum);
  CpxFloatArray_destroy(self->fft);
  CpxFloatArray_destroy(self->cepstrum);
  ffts_free(self->fftPlan);
  ffts_free(self->cepPlan);
  ffts_free(self->ifftPlan);
  free(self);
  self = NULL;

}







// float Pitch_estimate1(Pitch* self, float* data, int length) {

//   memcpy(self->audio, data, length * sizeof(float));

//   float max = 0;

//   for (int i = 0; i < self->fftLength; ++i) {
//     self->audio[i] *= self->window[i];
//   }

//   // Fourier transform
//   ffts_execute(self->fftPlan, self->audio, self->fft);


//   // find the strongest frequency bin

//   int bin = 0;
//   float value = 0.0;
//   float strongest = 0.0;

//   // find the strongest frequency bin
//   for (int i = 1; i < self->fftBinCount - 1; ++i) {


//     // TODO: instead of magnitude, just compare complex numbers?


//     value = magnitude(self->fft[i]);

//     if (value > strongest) {
//       strongest = value;
//       bin = i;
//     }

//   }

//   // calculate the weighted average

//   float mag, numerator = 0.0, denominator = 0.0;
//   float complex peak;

//   bin = bin + 2;
//   do {

//     // apply window (cheaper than windowing in the time domain)
//     // peak = blackmanFreq(
//     //   self->fft[bin-2],
//     //   self->fft[bin-1],
//     //   self->fft[bin],
//     //   self->fft[bin+1],
//     //   self->fft[bin+2]
//     // );

//     mag = magnitude(self->fft[bin]);
//     // mag = magnitude(peak);
//     numerator += mag * bin;
//     denominator += mag;

//   } while (--bin);

//   return (numerator / denominator) * self->freqPerBin;

// }
//


// autocorrelation (type 2)
static void acf2(Pitch* self) {

  // forward Fourier transform
  ffts_execute(self->fftPlan, self->audio.elements, self->fft.elements);

  // power spectrum
  for (int i = 0; i < self->fft.length; ++i) {
    self->fft.elements[i] = magnitude(self->fft.elements[i]);
  }

  // inverse Fourier transform --> autocorrelation
  ffts_execute(self->ifftPlan, self->fft.elements, self->acf.elements);

}


// Normalized square difference
static float nsdf(Pitch* self) {


  /* ----------------------------------------------------------------

    SDF formula (for lag t):

      d[t] = sum( (x[i] - x[i+t])ˆ2 )

      (sum is from 0 to W - 1 - t, 0 <= t < W)

    Relationship with autocorrelation:

      d[t] = sum( x[i]ˆ2 + x[i+t]ˆ2 ) - 2 * sum( x[i] * x[i+t] )
           = m(t) + 2r(t)

      m - left hand summation
      r - autocorrelation

    Normalized SDF or SNAC:

      nsdf = 2r / m    ( = 1 - sdf/m )

    ---------------------------------------------------------------- */

  acf2(self);

  int length = self->acf.length / 2;

  float norm = 1.0 / (float)length;

  // left-hand summation for zero lag
  float lhsum = norm * self->acf.elements[0];

  // normalized SDF (via autocorrelation)
  for (int t = 0; t < length; ++t) {

    if (lhsum > 0.0) {
      self->sdf.elements[t] = norm * self->acf.elements[t] / lhsum;
    }
    else {
      self->sdf.elements[t] = 0.0;
    }

    lhsum -= self->audio.elements[t] *
      self->audio.elements[t] +
      self->audio.elements[length-t-1] *
      self->audio.elements[length-t-1];

  }

  // choose peak

  int t = 1;
  int index = 0;
  int last = length - 1;
  int pos = 0;
  float peak = 0.0;

  // first negative zero crossing
  while (self->sdf.elements[t] > 0.0 && t < length) { ++t; }

  // loop over values below zero
  while (self->sdf.elements[t] < 0.0 && t < length) { ++t; }

  // find primary peaks
  while (t < last) {

    // find primary peak
    if (self->sdf.elements[t] > peak) {
      peak = self->sdf.elements[t];
      pos = t;
    }

    ++t;

    // negatively sloped zero crossing or the ending
    if (self->sdf.elements[t] < 0.0 || t == last) {
      peak = 0.0;
      self->primaryPeaks.elements[index] = pos;
      ++index;

      // loop over values below zero
      while (self->sdf.elements[t] < 0.0 && t < length) {
        ++t;
      }
    }

  }


  float threshold = 0.0;

  // find the largest primary peak value
  for (int i = 0; i < index; ++i) {
    int t = self->primaryPeaks.elements[i];
    if (self->sdf.elements[t] > threshold) {
      threshold = self->sdf.elements[t];
    }
  }

  threshold *= 0.9;
  int lag = 1;


  // first peak larger than the threshold
  for (int i = 0; i < index; ++i) {
    int t = self->primaryPeaks.elements[i];
    if (self->sdf.elements[t] > threshold) {
      lag = t;
      break;
    }
  }

  float delta = parabolic(self->sdf.elements[lag-1], self->sdf.elements[lag], self->sdf.elements[lag+1]);

  return self->samplerate / (lag + delta);

}


// static float cepstrum(Pitch* self) {

//   // // apply window
//   // for (int i = 0; i < self->fftLength; ++i) {
//   //   self->audio.elements[i] = self->audio.elements[i] * self->window.elements[i];
//   // }

//   // forward Fourier transform
//   ffts_execute(self->fftPlan, self->audio.elements, self->fft.elements);

//   // power spectrum
//   for (int i = 0; i < self->fft.length; ++i) {
//     self->powSpectrum.elements[i] = log10(magnitude(self->fft.elements[i]));
//   }

//   // inverse Fourier transform --> modified cepstrum
//   ffts_execute(self->cepPlan, self->powSpectrum.elements, self->cepstrum.elements);

//   for (int i = 0; i < self->cepstrum.length; ++i) {
//     self->powCepstrum.elements[i] = log10(magnitude(self->cepstrum.elements[i]));
//   }

//   return 0;


// }



float Pitch_estimate(Pitch* self, float* data) {

  memcpy(self->audio.elements, data, (self->audio.length / 2) * sizeof(float));
  return nsdf(self);

}


#undef PI