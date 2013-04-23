//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define CONFIG_MAX_PARTIALS 10
#define CONFIG_MAX_NOTES    50

typedef struct {
  int samplerate;
  int fftSamplerate;

  int fftLength;
  int bufferLength;
  int resampledBufferLength;

  int periodsPerFrame;
  double audioThreshold;
  double pitchStandard;
  bool displayFlats;
  int transpose;
  double centsOffset;

  // the same number of elements in partials and samplesPerPeriod arrays
  int partialsLength;
  int partials[CONFIG_MAX_PARTIALS];
  int samplesPerPeriod[CONFIG_MAX_PARTIALS];

  double instrumentNotes[CONFIG_MAX_NOTES];
} Config;


Config* Config_create();
void Config_destroy(Config* cfg);