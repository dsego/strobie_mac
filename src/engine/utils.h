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
inline static float to_dBFS(float value) {

  return 20 * log10(value);

}


inline static float from_dBFS(float value) {

  return pow(10, value / 20);

}



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


#undef SORT
#undef SWAP