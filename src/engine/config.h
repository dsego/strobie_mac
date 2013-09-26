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

  unsigned char color1[3];    // RGB color
  unsigned char color2[3];    // RGB color
  int bufferLength;           // audio buffer length
  int resampledLength;        // re-sampled buffer length
  float periodsPerFrame;      // wave cycles displayed in one frame
  int samplesPerPeriod;       // number of samples in a period
  float centsOffset;          // offset the note in cents
  float gain;                 // strobe intensity (sensitivity)
  StrobeMode mode;            //
  float value;                // frequency, multiplier or octave
  int subdivCount;              // sub-divide the strobe into multiple bands

} StrobeConfig;


typedef struct {

  int inputDevice;            // audio device index
  int outputDevice;           // audio device index
  int samplerate;             // input signal sampling rate
  int fftLength;              // length of FFT
  float pitchStandard;        // aka concert pitch ( A440 )
  int displayFlats;           // display Db instead of C#
  int transpose;              // shift notes to another key
  float centsOffset;          // offset in cents
  Note reference;
  float audioThreshold;       // TODO
  StrobeConfig strobes[CONFIG_MAX_STROBES];
  int strobeCount;

} Config;



Config* Config_create();
void Config_destroy(Config* self);