/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdbool.h>
#include "portaudio.h"
#include "Tuning.h"
#include "Config.h"
#include "Pitch.h"
#include "AudioFeed.h"
#include "Strobe.h"


#define MAX_STROBES 10
#define MAX_PITCHES 100



typedef struct {

  Config* config;             // configuration parameters
  PaStream* stream;           // PortAudio stream, provides access to audio hardware
  AudioFeed* audioFeed;       //
  Pitch* pitch;               // pitch recognition

  int pitchIndex;
  double pitches[MAX_PITCHES];

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