/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdbool.h>
#include "portaudio.h"
#include "Tuning.h"
#include "Config.h"
#include "Pitch.h"
#include "AudioFeed.h"
#include "Array.h"
#include "Strobe.h"


#define MAX_STROBES 10



typedef struct {

  Config* config;             // configuration parameters
  PaStream* stream;           // PortAudio stream, provides access to audio hardware
  AudioFeed* audioFeed;
  Pitch* pitch;               // pitch recognition

  FloatArray audioBuffer;
  float threshold;

  int strobeCount;
  Strobe* strobes[MAX_STROBES];
  FloatArray strobeBuffers[MAX_STROBES];

} Engine;




Engine* Engine_create();
void Engine_destroy(Engine* self);
bool Engine_startAudio(Engine* self);
void Engine_stopAudio(Engine* self);
void Engine_readStrobes(Engine* self);
void Engine_setStrobeFreq(Engine* self, float frequency);
float Engine_estimatePitch(Engine* self);