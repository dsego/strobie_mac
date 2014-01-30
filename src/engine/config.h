/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <assert.h>
#include "Tuning.h"



#define CONFIG_MAX_STROBES    16
#define CONFIG_MAX_COLORS     8
#define CONFIG_MAX_SCHEMES    8



typedef enum { OCTAVE, PARTIAL, FREQUENCY } StrobeMode;


// configuration options for each strobe band
typedef struct {

  int colors[CONFIG_MAX_COLORS][3];   // color stops (NOT USED)
  int bufferLength;                   // audio buffer length ( >= input buffer size )
  int resampledLength;                // re-sampled buffer length
  float periodsPerFrame;              // wave cycles displayed in one frame
  int samplesPerPeriod;               // number of samples in a period
  float centsOffset;                  // offset the note in cents
  StrobeMode mode;                    //
  float value;                        // frequency, multiplier or octave
  int subdivCount;                    // sub-divide the strobe into multiple bands

} StrobeConfig;


typedef struct {

  char name[64];
  float a[3];
  float b[3];

} ColorScheme;


typedef struct {

  int inputDevice;            // audio device index
  int inputBufferSize;        // buffer size determines latency
  int maxInputBufferSize;     // allocated memory
  int inputBufferSizes[64];   // buffer size options (zero-terminated)
  int outputDevice;           // audio device index
  int samplerate;             // input signal sampling rate
  int windowSize;             // audio window to process when estimating the pitch
  float pitchStandard;        // aka concert pitch ( A440 )
  int displayFlats;           // display Db instead of C#
  int transpose;              // shift notes to another key
  float centsOffset;          // offset in cents
  float freq;
  float clarityThreshold;
  float gain;
  float maxGain;              // NOT USED
  int highlightBands;         // highlight strong strobe bands and dim the weaker ones
  int strobeCount;
  int schemeIndex;
  int schemeCount;
  ColorScheme schemes[CONFIG_MAX_SCHEMES];
  StrobeConfig strobes[CONFIG_MAX_STROBES];

} Config;



Config* Config_create(void);
void Config_destroy(Config* self);
