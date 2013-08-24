/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


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
  double audioThreshold;
  double pitchStandard;
  bool displayFlats;
  int transpose;
  double centsOffset;
  int periodsPerFrame;
  int partialsLength;
  int partials[CONFIG_MAX_PARTIALS];
  int samplesPerPeriod[CONFIG_MAX_PARTIALS];
  double instrumentNotes[CONFIG_MAX_NOTES];

} Config;



Config* Config_create();
void Config_destroy(Config* self);