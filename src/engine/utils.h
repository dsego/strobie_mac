/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <complex.h>
#include <math.h>


/*
  Decibels relative to full scale (dBFS),
    measures decibel amplitude levels in digital systems
    which have a defined maximum available peak level.
 */
inline static double to_dBFS(double value) {

  return 20 * log10(value);

}


inline static double from_dBFS(double value) {

  return pow(10, value / 20);

}



inline static double min(int a, int b) {

  return (a < b) ? a : b;

}


inline static double minf(double a, double b) {

  return (a < b) ? a : b;

}


inline static double max(int a, int b) {

  return (a > b) ? a : b;

}


inline static double maxf(double a, double b) {

  return (a > b) ? a : b;

}


// limit a value
inline static double clamp(double val, double lower, double upper) {

  if (val < lower) {
    return lower;
  }
  else if (val > upper) {
    return upper;
  }
  else {
    return val;
  }

}


inline static double findWavePeak(float* buffer, int bufferLength) {

  double peak = 0;

  for (int i = 0; i < bufferLength; ++i) {
    if (buffer[i] < -peak) {
      peak = -buffer[i];
    }
    else if (buffer[i] > peak) {
      peak = buffer[i];
    }
  }

  return peak;

}


inline static double magnitude(float complex z) {

  return creal(z) * creal(z) + cimag(z) * cimag(z);

}


// find parabolic peak
inline static double parabolic(double a, double b, double c) {

  double bottom = a - 2.0 * b + c;
  if (bottom == 0.0) {
    return 0.0;
  }
  else {
    return 0.5 * (a - c) / bottom;
  }

}
