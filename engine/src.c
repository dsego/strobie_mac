/*
  Copyright (C) 2013 Davorin Šego
*/


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "src.h"



void SRC_set_ratio(SRC* src, double out_rate, double in_rate)
{
  src->ratio    = out_rate / in_rate;
  src->interval = 1.0 / src->ratio;
}

SRC* SRC_create(double out_rate, double in_rate)
{
  SRC* src = malloc(sizeof(SRC));
  assert(src != NULL);
  SRC_set_ratio(src, out_rate, in_rate);
  return src;
}

void SRC_destroy(SRC* src)
{
  free(src);
  src = NULL;
}

void SRC_reset(SRC* src)
{
  src->rems[0] = 0.0;
  src->rems[1] = 0.0;
  src->rems[2] = 0.0;
  src->time = 0;
}

void SRC_reset_indices(SRC* src)
{
  src->in_i  = (int) floor(src->time);
  src->out_i = 0;
}

void SRC_increment_time(SRC* src)
{
  src->time  += src->interval;
  src->out_i += 1;
  src->in_i  = (int) floor(src->time);
  src->t     = src->time - src->in_i;
}

double SRC_linear(double y0, double y1, double t)
{
  // t0 = 0, t1 = 1, t0 <= t <= t1
  return y0 + t * (y1 - y0);
}

double SRC_cubic(double y0, double y1, double y2, double y3, double t)
{
  /*
    Thanks to Paul Breeuwsma (http://www.paulinternet.nl/?page=bicubic)

    f(x)  = ax^3 + bx^2 + cx + d
    f'(x) = 3ax^2 + 2bx + c      (derivative)

    searching for y between y1 and y2:
    y1 = f(0), y2 = f(1)

    y1  = d
    y2  = a + b + c + d
    y1' = c
    y2' = 3a + 2b + c

    d = y1, c = y1'
    a = y2' + y1' - 2y2 + 2y1
    b = 3y2 - 3y1 -y2' - 2y1'

    derivatives for a smoother curve:
    y1' = (y2 - y0) / 2
    y2' = (y3 - y1) / 2

    d = y1
    c = (y2 - y0) / 2
    a = (-3/2)y2 + (3/2)y1 - (1/2)y0 + (1/2)y3
    b = 2y2 - (1/2)y3 + y0 - (5/2)y1

    final formula:

    f(t) = at^3 + bt^2 + ct + d
    f(t) = t^3 * ((-3/2)y2 + (3/2)y1 - (1/2)y0 + (1/2)y3) +
           t^2 * (2y2 - (1/2)y3 + y0 - (5/2)y1) +
           t   * ((y2 - y0) / 2) +
           y1
  */
  return y1 + ( ( ( (-y2 * 3/2) + (y1 * 3/2) - (y0 / 2) + (y3 / 2) ) * t
                + ((2 * y2)  - (y3 / 2) + y0 - (y1 * 5/2)) ) * t
                + ((y2 - y0) / 2) ) * t;
}


int SRC_linear_convert(SRC* src, float* in, int in_len, float* out, int out_len) {
  // Sanity check
  if ((int) (src->ratio * in_len) > out_len) {
    printf("Output buffer is too small (need %i, buffer size %i).\n", (int) (src->ratio * in_len), out_len);
    return 0;
  }

  SRC_reset_indices(src);

  // interval  before the first sample, from -1 to 0
  while (src->time < 0 && in_len >= 1) {
    out[src->out_i] = SRC_linear(src->rems[0], in[0], src->time + 1);
    SRC_increment_time(src);
  }

  int last = in_len - 1;

  // For every two (relevant) input samples calculate the output
  while (src->in_i < last) {
    out[src->out_i] = SRC_linear(in[src->in_i], in[src->in_i + 1], src->t);
    SRC_increment_time(src);
  }

  // Save the last sample
  src->rems[0] = in[last];

  // Wrap around
  src->time = src->time - last - 1;

  // Number of generated samples
  return src->out_i;

}

int SRC_cubic_convert(SRC* src, float* in, int in_len, float* out, int out_len) {

  // Sanity check
  if ((int) (src->ratio * in_len) > out_len) {
    printf("Output buffer is too small (need %i, buffer size %i).\n", (int) (src->ratio * in_len), out_len);
    return 0;
  }

  SRC_reset_indices(src);

  //  from -2 to -1
  while (src->time < -1 && in_len >= 1) {
    out[src->out_i] = SRC_cubic(src->rems[0], src->rems[1], src->rems[2], in[0], src->time + 2);
    SRC_increment_time(src);
  }

  // interval before the first sample, from -1 to 0
  while (src->time < 0 && in_len >= 2) {
    out[src->out_i] = SRC_cubic(src->rems[1], src->rems[2],  in[0], in[1], src->time + 1);
    SRC_increment_time(src);
  }

  // interval from 0 to 1
  while (src->time < 1 && in_len >= 3) {
    out[src->out_i] = SRC_cubic(src->rems[2], in[0], in[1], in[2], src->time);
    SRC_increment_time(src);
  }

  int last = in_len - 2;

  while (src->in_i < last) {
    out[src->out_i] = SRC_cubic(in[src->in_i - 1], in[src->in_i], in[src->in_i + 1], in[src->in_i + 2], src->t);
    SRC_increment_time(src);
  }

  // save the last 3 samples for next call
  src->rems[0] = in[last - 1];
  src->rems[1] = in[last];
  src->rems[2] = in[last + 1];

  // wrap around
  src->time = src->time - last - 2;

  // Number of generated samples
  return src->out_i;

}