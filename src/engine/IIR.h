/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
 */


#define MAX_ARRAY_SIZE 10


typedef struct {

  int numSecondOrderStages;
  int firstOrderStage;

  double w[MAX_ARRAY_SIZE][3];
  double a[MAX_ARRAY_SIZE][3];
  double b[MAX_ARRAY_SIZE][3];

} IIR;


IIR* IIR_CreateHalfbandEllip();
IIR* IIR_CreateHalfbandCheby();
IIR* IIR_CreateQuartbandEllip();
IIR* IIR_CreateQuartbandCheby();
IIR* IIR_CreateThreePercEllip();
IIR* IIR_CreateThreePercCheby();

void IIR_filter(IIR* self, float* input, float* output, int length);
void IIR_reset(IIR* self);