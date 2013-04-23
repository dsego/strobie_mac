//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//


// Sample rate conversion
//  interpolates the siglan in time domain
//  (supports streaming data and arbitrary conversion factors)
typedef struct {
  double ratio;         // output sample rate / input sample rate
  double interval;      // time distance between generated (output) samples
  double rems[3];       // remaining samples
  double outPos;        // position of the output sample between two input samples
  int inIndex;          // input index
  int outIndex;         // output index
  double outTime;       // output time (incremented by interval = 1 / ratio )
} SRC;


SRC* SRC_create(double outRate, double inRate);
void SRC_destroy(SRC* src);

void SRC_setRatio(SRC* src, double outRate, double inRate);
void SRC_reset(SRC* src);



// Convert a chunk of data - linear interpolation.
// Returns the number of generated samples.
int SRC_linearConvert(SRC* src, float* in, int inLength, float* out, int outLength);


// Convert a chunk of data - cubic interpolation.
// Returns the number of generated samples.

int SRC_cubicConvert(SRC* src, float* in, int inLength, float* out, int outLength);