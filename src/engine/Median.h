/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */

#include <stdlib.h>
#include <assert.h>


#ifndef MEDIAN_H
#define MEDIAN_H


typedef struct {

  int index;
  int size;
  float elements[32];

} Median;




Median* Median_create(int size);
void Median_destroy(Median *self);
void Median_push(Median* self, float value);
float Median_value(Median* self);


#endif
