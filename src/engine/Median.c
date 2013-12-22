/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include "Median.h"


Median* Median_create(int size) {

  Median* self = malloc(sizeof(Median));
  assert(self);

  self->index = 0;
  self->size = 3;
  return self;

}


void Median_destroy(Median* self) {

  free(self);

}


void Median_push(Median* self, float value) {

  self->elements[self->index] = value;
  self->index += 1;
  if (self->index > self->size) {
    self->index = 0;
  }

}


static inline float median3(float *p);
static inline float median5(float *p);
static inline float median7(float *p);
static inline float median9(float *p);
static inline float median25(float *p);


float Median_value(Median* self) {

  switch (self->size) {
    case 3:
      return median3(self->elements);
    case 5:
      return median5(self->elements);
    case 7:
      return median7(self->elements);
    case 9:
      return median9(self->elements);
    case 25:
      return median25(self->elements);
    default:
      return 0;
  };

}




// http://ndevilla.free.fr/median/median/src/optmed.c

#define SORT(a,b) { if ((a)>(b)) SWAP((a),(b)); }
#define SWAP(a,b) { float temp=(a);(a)=(b);(b)=temp; }



static inline float median3(float *p) {

  SORT(p[0],p[1]);
  SORT(p[1],p[2]);
  SORT(p[0],p[1]);
  return(p[1]);

}


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


static inline float median7(float *p) {

  SORT(p[0], p[5]);
  SORT(p[0], p[3]);
  SORT(p[1], p[6]);
  SORT(p[2], p[4]);
  SORT(p[0], p[1]);
  SORT(p[3], p[5]);
  SORT(p[2], p[6]);
  SORT(p[2], p[3]);
  SORT(p[3], p[6]);
  SORT(p[4], p[5]);
  SORT(p[1], p[4]);
  SORT(p[1], p[3]);
  SORT(p[3], p[4]);
  return (p[3]);

}


static inline float median9(float *p) {

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
  return(p[4]);

}


static inline float median25(float *p) {

  SORT(p[0], p[1]);
  SORT(p[3], p[4]);
  SORT(p[2], p[4]);
  SORT(p[2], p[3]);
  SORT(p[6], p[7]);
  SORT(p[5], p[7]);
  SORT(p[5], p[6]);
  SORT(p[9], p[10]);
  SORT(p[8], p[10]);
  SORT(p[8], p[9]);
  SORT(p[12], p[13]);
  SORT(p[11], p[13]);
  SORT(p[11], p[12]);
  SORT(p[15], p[16]);
  SORT(p[14], p[16]);
  SORT(p[14], p[15]);
  SORT(p[18], p[19]);
  SORT(p[17], p[19]);
  SORT(p[17], p[18]);
  SORT(p[21], p[22]);
  SORT(p[20], p[22]);
  SORT(p[20], p[21]);
  SORT(p[23], p[24]);
  SORT(p[2], p[5]);
  SORT(p[3], p[6]);
  SORT(p[0], p[6]);
  SORT(p[0], p[3]);
  SORT(p[4], p[7]);
  SORT(p[1], p[7]);
  SORT(p[1], p[4]);
  SORT(p[11], p[14]);
  SORT(p[8], p[14]);
  SORT(p[8], p[11]);
  SORT(p[12], p[15]);
  SORT(p[9], p[15]);
  SORT(p[9], p[12]);
  SORT(p[13], p[16]);
  SORT(p[10], p[16]);
  SORT(p[10], p[13]);
  SORT(p[20], p[23]);
  SORT(p[17], p[23]);
  SORT(p[17], p[20]);
  SORT(p[21], p[24]);
  SORT(p[18], p[24]);
  SORT(p[18], p[21]);
  SORT(p[19], p[22]);
  SORT(p[8], p[17]);
  SORT(p[9], p[18]);
  SORT(p[0], p[18]);
  SORT(p[0], p[9]);
  SORT(p[10], p[19]);
  SORT(p[1], p[19]);
  SORT(p[1], p[10]);
  SORT(p[11], p[20]);
  SORT(p[2], p[20]);
  SORT(p[2], p[11]);
  SORT(p[12], p[21]);
  SORT(p[3], p[21]);
  SORT(p[3], p[12]);
  SORT(p[13], p[22]);
  SORT(p[4], p[22]);
  SORT(p[4], p[13]);
  SORT(p[14], p[23]);
  SORT(p[5], p[23]);
  SORT(p[5], p[14]);
  SORT(p[15], p[24]);
  SORT(p[6], p[24]);
  SORT(p[6], p[15]);
  SORT(p[7], p[16]);
  SORT(p[7], p[19]);
  SORT(p[13], p[21]);
  SORT(p[15], p[23]);
  SORT(p[7], p[13]);
  SORT(p[7], p[15]);
  SORT(p[1], p[9]);
  SORT(p[3], p[11]);
  SORT(p[5], p[17]);
  SORT(p[11], p[17]);
  SORT(p[9], p[17]);
  SORT(p[4], p[10]);
  SORT(p[6], p[12]);
  SORT(p[7], p[14]);
  SORT(p[4], p[6]);
  SORT(p[4], p[7]);
  SORT(p[12], p[14]);
  SORT(p[10], p[14]);
  SORT(p[6], p[7]);
  SORT(p[10], p[12]);
  SORT(p[6], p[10]);
  SORT(p[6], p[17]);
  SORT(p[12], p[17]);
  SORT(p[7], p[17]);
  SORT(p[7], p[10]);
  SORT(p[12], p[18]);
  SORT(p[7], p[12]);
  SORT(p[10], p[18]);
  SORT(p[12], p[20]);
  SORT(p[10], p[20]);
  SORT(p[10], p[12]);
  return (p[12]);

}


#undef SORT
#undef SWAP
