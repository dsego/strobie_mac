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



Pitch* Pitch_create(int samplerate, int windowSize) {

  Pitch* self = malloc(sizeof(Pitch));
  assert(self != NULL);

  self->samplerate = samplerate;

  // zero-pad (autocorrelation)
  int paddedLength = windowSize + windowSize;

  // The output of a real-to-complex transform is N/2+1 complex numbers
  int fftBinCount = windowSize + 1;

  self->fftPlan = ffts_init_1d_real(paddedLength, -1);
  self->ifftPlan = ffts_init_1d_real(paddedLength, 1);

  self->fft   = CpxFloatArray_create(fftBinCount);
  self->audio = FloatArray_create(paddedLength);
  self->nsdf  = FloatArray_create(paddedLength);
  // self->window = FloatArray_create(paddedLength);
  // self->powSpectrum = FloatArray_create(fftBinCount);
  // blackman(self->window.elements, paddedLength);

  // Cepstrum
  // int cepLength = paddedLength / 2;
  // int cepBinCount = cepLength / 2;
  // self->cepPlan = ffts_init_1d_real(cepLength, -1);
  // self->cepstrum = CpxFloatArray_create(cepBinCount);
  // self->powCepstrum = FloatArray_create(cepBinCount);

  // yeah, yeah, calloc, bla bla
  FloatArray_fill(self->audio, 0.0);

  return self;

}


void Pitch_destroy(Pitch* self) {

  FloatArray_destroy(self->audio);
  FloatArray_destroy(self->nsdf);
  CpxFloatArray_destroy(self->fft);
  ffts_free(self->fftPlan);
  ffts_free(self->ifftPlan);
  // FloatArray_destroy(self->window);
  // FloatArray_destroy(self->powSpectrum);
  // FloatArray_destroy(self->powCepstrum);
  // CpxFloatArray_destroy(self->cepstrum);
  // ffts_free(self->cepPlan);
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


inline static void findLag(Pitch* self, float *nsdf, int length, float *outLag, float *outAmp);


// Normalized square difference
inline static void nsdf(Pitch* self, float *outLag, float *outAmp) {

  // autocorrelation:  FFT -> power spectrum -> inverse FFT
  ffts_execute(self->fftPlan, self->audio.elements, self->fft.elements);

  for (int i = 0; i < self->fft.length; ++i) {
    self->fft.elements[i] = magnitude(self->fft.elements[i]);
  }

  ffts_execute(self->ifftPlan, self->fft.elements, self->nsdf.elements);

  // NSDF (SNAC)
  int n = self->nsdf.length / 2;

  // TODO - moves this to configuration !
  int k = n - 256;
  float norm = 1.0 / (float) self->nsdf.length;
  float *audio = self->audio.elements;

  // left-hand summation for zero lag
  double lhsum = 2.0 * (self->nsdf.elements[0] * norm);

  // normalized SDF (via autocorrelation)
  for (int i = 0; i < k; ++i) {

    if (lhsum > 0.0) {
      self->nsdf.elements[i] *= norm * 2.0 / lhsum;
    }
    else {
      self->nsdf.elements[i] = 0.0;
    }

    lhsum -= audio[i] * audio[i] + audio[n-i-1] * audio[n-i-1];

  }

  findLag(self, self->nsdf.elements, k, outLag, outAmp);

}


inline static void findLag(Pitch* self, float *data, int length, float *outLag, float *outAmp) {

  int t = 1;
  int last = length - 1;

  int index = 0;
  int peakPos = 0;
  int largestPeakPos = 0;

  #define PP_LENGTH 512
  int primaryPeaks[PP_LENGTH];


  // ---- find all primary peaks ----
  //
  //  primary peak is the highest peak
  //    between a pair of positive zero crossings
  //

  // first negative zero crossing
  while (data[t] > 0.0 && t < last/2) { ++t; }

  // loop over negative values
  while (data[t] <= 0.0 && t < last) { ++t; }

  // first positive zero crossing

  // find primary peaks
  while (t < last) {

    // primary peak
    if (data[t-1] < data[t] && data[t] >= data[t+1]) {
      if (peakPos == 0 || data[t] > data[peakPos]) {
        peakPos = t;
      }
    }

    ++t;

    // negatively sloped zero crossing
    if (data[t] <= 0.0 && t < last) {

      // there was a maximum
      if (peakPos > 0 && index < PP_LENGTH) {
        primaryPeaks[index] = peakPos;

        // overall largest primary peak
        if (largestPeakPos == 0 || data[peakPos] > data[largestPeakPos]) {
          largestPeakPos = peakPos;
        }
        peakPos = 0;
        ++index;
      }

      // loop over negative values
      while (data[t] <= 0.0 && t < last) { ++t; }

    }

  }

  // there was a peak in the last part
  if (peakPos > 0 && index < PP_LENGTH) {
    primaryPeaks[index] = peakPos;
    if (largestPeakPos == 0 || data[peakPos] > data[largestPeakPos]) {
      largestPeakPos = peakPos;
    }
    ++index;
  }

  *outLag = 0.0;
  *outAmp = 0.0;

  // choose the appropriate primary peak
  if (index > 0) {

    const float c = 0.8;
    float threshold = c * data[largestPeakPos];
    float delta = 0.0;
    float amp = 0.0;
    int lag = 1;

    // first peak larger than the threshold
    for (int i = 0; i < index; ++i) {
      int pos = primaryPeaks[i];
      if (data[pos] >= threshold) {
        lag = primaryPeaks[i];
        break;
      }
    }


    parabolic(data[lag-1], data[lag], data[lag+1], &delta, &amp);
    *outLag = lag + delta;
    // printf("%i        %6.4f  %8.4f  \r", lag, lag + delta, 44100.0 / ((double)lag + delta));fflush(stdout);
    *outAmp = amp;

  }


  #undef PP_LENGTH

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



void Pitch_estimate(Pitch* self, float* data, float *outFreq, float *outClarity) {

  // padded audio data
  memcpy(self->audio.elements, data, (self->audio.length / 2) * sizeof(float));

  float lag, clarity;
  nsdf(self, &lag, &clarity);

  *outFreq = (lag > 2.0) ? self->samplerate / lag : 0.0; // anything less than 2 is probably an error
  *outClarity = (clarity >= 0.0) ? clarity : 0.0;

}


#undef PI
