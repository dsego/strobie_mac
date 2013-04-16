/*
  Copyright (C) 2013 Davorin Šego
*/

typedef struct {
  double ratio;         // output sample rate / input sample rate
  double interval;      // time distance between generated samples
  double rems[3];       // remaining samples
  double t;             // position of the output sample between two input samples
  int in_i;             // input index
  int out_i;            // output index
  double time;          // output time
} SRC;


SRC* SRC_create(double out_rate, double in_rate);
void SRC_destroy(SRC* src);

void SRC_set_ratio(SRC* src, double out_rate, double in_rate);
void SRC_reset(SRC* src);


/**
 * Convert a chunk of data - linear interpolation.
 * Returns the number of generated samples.
 */
int SRC_linear_convert(SRC* src, float* in, int in_len, float* out, int out_len);

/**
 * Convert a chunk of data - cubic interpolation.
 * Returns the number of generated samples.
 */
int SRC_cubic_convert(SRC* src, float* in, int in_len, float* out, int out_len);