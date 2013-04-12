/*
  Copyright (C) 2013 Davorin Šego
*/

//
//  IIR Biquad filter
//
//  Biquad formulas from:
//    Nigel Redmon
//    http://www.earlevel.com/main/2011/01/02/biquad-formulas/
//

static const int MAX_SECTION_COUNT = 10;

typedef struct {

  // delay line
  double z1[MAX_SECTION_COUNT];
  double z2[MAX_SECTION_COUNT];

  // cascade multiple biquads
  int sectionCount;

  // coefficients
  double a0;
  double a1;
  double a2;
  double b1;
  double b2;

} Biquad;



Biquad* Biquad_create(int sectionCount);
void Biquad_destroy(Biquad* bq);


// redefine filter coefficients
void Biquad_lowpass(Biquad* bq, double cutoff, double samplerate, double Q);
void Biquad_highpass(Biquad* bq, double cutoff, double samplerate, double Q);
void Biquad_bandpass(Biquad* bq, double cutoff, double samplerate, double Q);

// reset the delay line
void Biquad_reset(Biquad* bq);

// filter audio
void Biquad_filter(Biquad* bq, double* input, int input_len, double* output, int output_len);