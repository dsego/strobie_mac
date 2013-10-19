/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <complex.h>
#include <math.h>




inline static float min(int a, int b) {

  return (a < b) ? a : b;

}


inline static float minf(float a, float b) {

  return (a < b) ? a : b;

}


inline static float max(int a, int b) {

  return (a > b) ? a : b;

}


inline static float maxf(float a, float b) {

  return (a > b) ? a : b;

}


// limit a value
inline static float clamp(float val, float lower, float upper) {

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


inline static float findWavePeak(float* buffer, int bufferLength) {

  float peak = 0;

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


inline static float magnitude(float complex z) {

  return creal(z) * creal(z) + cimag(z) * cimag(z);

}


// find parabolic peak
inline static float parabolic(float a, float b, float c) {

  float bottom = a - 2.0 * b + c;
  if (bottom == 0.0) {
    return 0.0;
  }
  else {
    return 0.5 * (a - c) / bottom;
  }

}


// http://ndevilla.free.fr/median/median/src/optmed.c

#define SORT(a,b) { if ((a)>(b)) SWAP((a),(b)); }
#define SWAP(a,b) { float temp=(a);(a)=(b);(b)=temp; }


static inline float median5(float *p) {

  SORT(p[0],p[1]);
  SORT(p[3],p[4]);
  SORT(p[0],p[3]);
  SORT(p[1],p[4]);
  SORT(p[1],p[2]);
  SORT(p[2],p[3]);
  SORT(p[1],p[2]);
  return(p[2]);

}


static inline float median9(float * p) {

  SORT(p[1], p[2]);
  SORT(p[4], p[5]);
  SORT(p[7], p[8]);
  SORT(p[0], p[1]);
  SORT(p[3], p[4]);
  SORT(p[6], p[7]);
  SORT(p[1], p[2]);
  SORT(p[4], p[5]);
  SORT(p[7], p[8]);
  SORT(p[0], p[3]);
  SORT(p[5], p[8]);
  SORT(p[4], p[7]);
  SORT(p[3], p[6]);
  SORT(p[1], p[4]);
  SORT(p[2], p[5]);
  SORT(p[4], p[7]);
  SORT(p[4], p[2]);
  SORT(p[6], p[4]);
  SORT(p[4], p[2]);
  return(p[4]) ;

}



static inline void hsv2rgb(int inHSV[3], unsigned char outRGB[3]) {

  float h = inHSV[0];
  float s = inHSV[1] * 0.01;
  float v = inHSV[2] * 0.01;

  float r = 0;
  float g = 0;
  float b = 0;

  float c = s * v; // chroma
  float x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));

  if (h < 60) {
    r = c;
    g = x;
  }
  else if (h < 120) {
    r = x;
    g = c;
  }
  else if (h < 180) {
    g = c;
    b = x;
  }
  else if (h < 240) {
    g = x;
    b = c;
  }
  else if (h < 300) {
    r = x;
    b = c;
  }
  else if (h < 360) {
    r = c;
    b = x;
  }

  float m = v - c;

  outRGB[0] = roundf((r + m) * 255);
  outRGB[1] = roundf((g + m) * 255);
  outRGB[2] = roundf((b + m) * 255);

}


#undef SORT
#undef SWAP