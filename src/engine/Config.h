/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "Tuning.h"



#define CONFIG_MAX_STROBES   10



typedef enum { OCTAVE, PARTIAL, FREQUENCY } StrobeMode;


// configuration options for each strobe band
typedef struct {

  int color1[3];    // color stop
  int color2[3];    // color stop
  int color3[3];    // color stop

  int bufferLength;           // audio buffer length
  int resampledLength;        // re-sampled buffer length
  float periodsPerFrame;      // wave cycles displayed in one frame
  int samplesPerPeriod;       // number of samples in a period
  float centsOffset;          // offset the note in cents
  float gain;                 // strobe intensity (sensitivity)
  StrobeMode mode;            //
  float value;                // frequency, multiplier or octave
  int subdivCount;            // sub-divide the strobe into multiple bands

} StrobeConfig;


typedef struct {

  int inputDevice;            // audio device index
  int inputBufferSize;        // buffer size determines latency
  int outputDevice;           // audio device index
  int samplerate;             // input signal sampling rate
  int windowSize;             // audio window to process when estimating the pitch
  float pitchStandard;        // aka concert pitch ( A440 )
  int displayFlats;           // display Db instead of C#
  int transpose;              // shift notes to another key
  float centsOffset;          // offset in cents
  float freq;
  float pitchDetectionUpperThreshold;
  float pitchDetectionLowerThreshold;
  StrobeConfig strobes[CONFIG_MAX_STROBES];
  int strobeCount;

} Config;



Config* Config_create(void);
void Config_destroy(Config* self);
