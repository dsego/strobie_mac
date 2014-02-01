/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */


#ifndef VEC_H
#define VEC_H


typedef struct {

  void *elements;
  int capacity;   // max. number of elements
  int size;       // element size
  int count;      // number of elements

} Vec;


Vec* Vec_create(int capacity, int size);
void Vec_destroy(Vec* vec);


#endif
