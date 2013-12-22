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



// static inline void hsv2rgb(int inHSV[3], unsigned char outRGB[3]) {

//   float h = inHSV[0];
//   float s = inHSV[1] * 0.01;
//   float v = inHSV[2] * 0.01;

//   float r = 0;
//   float g = 0;
//   float b = 0;

//   float c = s * v; // chroma
//   float x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));

//   if (h < 60) {
//     r = c;
//     g = x;
//   }
//   else if (h < 120) {
//     r = x;
//     g = c;
//   }
//   else if (h < 180) {
//     g = c;
//     b = x;
//   }
//   else if (h < 240) {
//     g = x;
//     b = c;
//   }
//   else if (h < 300) {
//     r = x;
//     b = c;
//   }
//   else if (h < 360) {
//     r = c;
//     b = x;
//   }

//   float m = v - c;

//   outRGB[0] = roundf((r + m) * 255);
//   outRGB[1] = roundf((g + m) * 255);
//   outRGB[2] = roundf((b + m) * 255);

// }


#undef SORT
#undef SWAP
