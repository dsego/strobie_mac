/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#ifndef FIND_LAG
#define FIND_LAG


inline static void findLag(float *data, int length, float threshold, float *outLag, float *outAmp) {

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

    // float threshold = data[largestPeakPos] + (c - 1.0) * data[largestPeakPos] * data[largestPeakPos];
    // float threshold = c + (1.0 - c) * (1.0 - data[largestPeakPos]);
    threshold = threshold * data[largestPeakPos];

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
    // printf(" %4i  %4i    \r", lag, largestPeakPos);fflush(stdout);
    *outAmp = amp;

  }

  #undef PP_LENGTH

}

#endif
