/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdbool.h>
#include "portaudio.h"
#include "Tuning.h"
#include "Config.h"
#include "PitchEstimation.h"
#include "AudioFeed.h"
#include "Strobe.h"

#define MAX_STROBES 10

typedef struct {

  Config* config;
  PaStream* stream;
  AudioFeed* audioFeed;

  PitchEstimation* pitchEstimation;
  float* audioBuffer;
  double threshold;

  Strobe* strobes[MAX_STROBES];
  int strobeCount;
  float* strobeBuffers[MAX_STROBES];
  int strobeBufferLengths[MAX_STROBES];

} Engine;


Engine* Engine_create();
void Engine_destroy(Engine* engine);

bool Engine_startAudio(Engine* engine);
void Engine_stopAudio(Engine* engine);

void Engine_readStrobes(Engine* engine);
void Engine_setStrobeFreq(Engine* engine, double frequency);
double Engine_estimatePitch(Engine* engine);