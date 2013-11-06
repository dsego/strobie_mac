/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <complex.h>

#ifndef ARRAY_H
#define ARRAY_H


typedef struct {

  int *elements;
  int length;

} IntArray;


typedef struct {

  float *elements;
  int length;

} FloatArray;


typedef struct {

  float complex *elements;
  int length;

} CpxFloatArray;



IntArray IntArray_create(int length);
FloatArray FloatArray_create(int length);
CpxFloatArray CpxFloatArray_create(int length);

void IntArray_fill(IntArray array, int value);
void FloatArray_fill(FloatArray array, float value);
void CpxFloatArray_fill(CpxFloatArray array, float complex value);

void IntArray_destroy(IntArray array);
void FloatArray_destroy(FloatArray array);
void CpxFloatArray_destroy(CpxFloatArray array);


#endif
