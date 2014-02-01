/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */



#include <stdlib.h>
#include <assert.h>
#include <complex.h>
#include "Vec.h"


Vec* Vec_create(int capacity, int size) {

  Vec* self = malloc(sizeof(Vec));
  assert(self != NULL);

  self->elements = malloc(capacity * size);
  assert(self->elements != NULL);

  self->size = size;
  self->capacity = capacity;
  self->count = capacity;
  return self;

}


void Vec_destroy(Vec* self) {

  free(self->elements);
  free(self);

}
