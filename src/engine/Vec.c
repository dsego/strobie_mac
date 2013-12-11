/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/



#include <stdlib.h>
#include <assert.h>
#include <complex.h>
#include "Vec.h"


Vec Vec_create(const int capacity, const int size) {

  Vec vec;
  vec.elements = malloc(capacity * size);
  assert(vec.elements != NULL);

  vec.size = size;
  vec.capacity = capacity;
  vec.count = capacity;
  return vec;

}


void Vec_destroy(Vec vec) {

  free(vec.elements);
  vec.size = 0;
  vec.capacity = 0;
  vec.count = 0;

}
