/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>


#define CONFIG_MAX_PARTIALS 10
#define CONFIG_MAX_NOTES    50



typedef struct {

  int deviceIndex;
  int samplerate;
  int bufferLength;
  int resampledLength;
  int averageCount;
  int fftSamplerate;
  int fftLength;
  float audioThreshold;
  float strobeGain;
  float pitchStandard;
  char displayFlats;
  int transpose;
  float centsOffset;
  unsigned char strobeColor1[3];
  unsigned char strobeColor2[3];
  int periodsPerFrame;
  int partialsCount;
  int partials[CONFIG_MAX_PARTIALS];
  int samplesPerPeriod[CONFIG_MAX_PARTIALS];
  float instrumentNotes[CONFIG_MAX_NOTES];

} Config;



Config* Config_create();
void Config_destroy(Config* self);