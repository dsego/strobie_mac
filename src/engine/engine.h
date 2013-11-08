/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#ifndef ENGINE_H
#define ENGINE_H


#include "portaudio.h"
#include "Tuning.h"
#include "Config.h"
#include "Pitch.h"
#include "AudioFeed.h"
#include "Array.h"
#include "Strobe.h"



#define MAX_STROBES CONFIG_MAX_STROBES


typedef enum { AUTO, MANUAL } DetectionMode;


typedef struct {

  Config* config;             // configuration parameters
  PaStream* stream;           // PortAudio stream, provides access to audio hardware
  int paInitFailed;           // if Pa_Initialize() returns an error Pa_Terminate() should NOT be called
  AudioFeed* audioFeed;
  Pitch* pitch;               // pitch recognition

  FloatArray audioBuffer;
  float level;

  DetectionMode mode;
  Note currentNote;

  int strobeCount;
  Strobe* strobes[MAX_STROBES];
  FloatArray strobeBuffers[MAX_STROBES];
  int strobeLengths[MAX_STROBES];

} Engine;




Engine* Engine_create(void);
void Engine_destroy(Engine* self);

int Engine_setInputDevice(Engine *self, int device, int samplerate);
int Engine_readStrobes(Engine* self);
float Engine_estimatePitch(Engine* self);

void Engine_setStrobes(Engine* self, Note note);
void Engine_setCentsOffset(Engine* self, float cents);
void Engine_setPitchStandard(Engine* self, float freq);
void Engine_setGain(Engine* self, float gain);
void Engine_setColors(Engine* self, int color1[3], int color2[3]);

void Engine_getSamplerates(int device, int outSamplerates[11]);
int Engine_getDefaultInputDevice(void);
int Engine_deviceCount(void);
int Engine_deviceName(int index, char *outName, int *outIsInput, int *outIsOutput);


#endif
