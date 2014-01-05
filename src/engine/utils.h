/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <complex.h>
#include <math.h>




inline static float min(const int a, const int b) {

  return (a < b) ? a : b;

}


inline static float minf(const float a, const float b) {

  return (a < b) ? a : b;

}


inline static float max(const int a, const int b) {

  return (a > b) ? a : b;

}


inline static float maxf(const float a, const float b) {

  return (a > b) ? a : b;

}


// limit a value
inline static float clamp(const float val, const float lower, const float upper) {

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


inline static float findWavePeak(const float* buffer, const int bufferLength) {

  float peak = 0;

  for (int i = 0; i < bufferLength; ++i) {
    float pk = fabs(buffer[i]);
    if (pk > peak) {
      peak = pk;
    }
  }

  return peak;

}


inline static float magnitude(const float complex z) {

  return creal(z) * creal(z) + cimag(z) * cimag(z);

}


// find parabolic peak
// x - interpolated peak location
// y - peak magnitude estimate
inline static void parabolic(const double a, const double b, const double c, float *outX, float *outY) {

  double bottom = 2.0 * (a - 2.0 * b + c);
  if (fabs(bottom) < 0.00001) { // bottom == 0.0
    *outY = b;
    *outX = 0.0;
  }
  else {
    *outX = (a - c) / bottom;
    *outY = b - 0.25 * (a - c) * (*outX);
  }

}

// HSV to RGB conversion
//  http://en.wikipedia.org/wiki/HSV_color_space#Conversion_from_RGB_to_HSL_or_HSV

static inline void hsv2rgb(const float hsv[3], float outRGB[3]) {

  float c = hsv[1] * hsv[2]; // chroma
  float hdash = hsv[0] * 6.0;
  float x = c * (1 - fabs(fmod(hdash, 2) - 1));

  if (hdash < 1) {
    outRGB[0] = c;
    outRGB[1] = x;
    outRGB[2] = 0;
  }
  else if (hdash < 2) {
    outRGB[0] = x;
    outRGB[1] = c;
    outRGB[2] = 0;
  }
  else if (hdash < 3) {
    outRGB[0] = 0;
    outRGB[1] = c;
    outRGB[2] = x;
  }
  else if (hdash < 4) {
    outRGB[0] = 0;
    outRGB[1] = x;
    outRGB[2] = c;
  }
  else if (hdash < 5) {
    outRGB[0] = x;
    outRGB[1] = 0;
    outRGB[2] = c;
  }
  else {
    outRGB[0] = c;
    outRGB[1] = 0;
    outRGB[2] = x;
  }

  float m = hsv[2] - c;

  outRGB[0] += m;
  outRGB[1] += m;
  outRGB[2] += m;

}


#undef SORT
#undef SWAP
