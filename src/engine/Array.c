/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/



#include <stdlib.h>
#include <assert.h>
#include "Array.h"


IntArray IntArray_create(int length) {

  IntArray array;
  array.elements = malloc(length * sizeof(int));
  assert(array.elements != NULL);
  array.length = length;
  return array;

}


FloatArray FloatArray_create(int length) {

  FloatArray array;
  array.elements = malloc(length * sizeof(float));
  assert(array.elements != NULL);
  array.length = length;
  return array;

}


CpxFloatArray CpxFloatArray_create(int length) {

  CpxFloatArray array;
  array.elements = malloc(length * sizeof(float complex));
  assert(array.elements != NULL);
  array.length = length;
  return array;

}


void IntArray_fill(IntArray array, int value) {

  int i = array.length-1;
  do { array.elements[i] = value; } while(--i);

}


void FloatArray_fill(FloatArray array, float value) {

  int i = array.length-1;
  do { array.elements[i] = value; } while(--i);

}


void CpxFloatArray_fill(CpxFloatArray array, float complex value) {

  int i = array.length-1;
  do { array.elements[i] = value; } while(--i);

}


void IntArray_destroy(IntArray array) {

  free(array.elements);
  array.length = 0;

}


void FloatArray_destroy(FloatArray array) {

  free(array.elements);
  array.length = 0;

}


void CpxFloatArray_destroy(CpxFloatArray array) {

  free(array.elements);
  array.length = 0;

}