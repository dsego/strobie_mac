/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <math.h>


/*
  Decibels relative to full scale (dBFS),
    measures decibel amplitude levels in digital systems
    which have a defined maximum available peak level.
 */
double to_dBFS(double value) {

  return 20 * log10(value);

}


double from_dBFS(double value) {

  return pow(10, value / 20);

}



double min(int a, int b) {

  return (a < b) ? a : b;

}


double minf(double a, double b) {

  return (a < b) ? a : b;

}


double max(int a, int b) {

  return (a > b) ? a : b;

}


double maxf(double a, double b) {

  return (a > b) ? a : b;

}


// limit a value
double clamp(double val, double lower, double upper) {

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


double findPeak(float* buffer, int bufferLength) {

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


// Parabolic interpolation
double parabolaPeakPosition(double a, double b, double c) {

  return (a - c) / (2 * (a - 2 * b + c));

}