// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.


#ifndef ENGINE_H
#define ENGINE_H


#include <pthread.h>
#include "portaudio.h"
#include "equal-temp.h"
#include "config.h"
#include "nsdf.h"
#include "audio-feed.h"
#include "median.h"
#include "buffer.h"
#include "strobe.h"



#define MAX_STROBES CONFIG_MAX_STROBES

typedef PaDeviceInfo DeviceInfo;


typedef enum {
  AUTO,
  MANUAL
} DetectionMode;


typedef struct {

  Config* config;             // configuration parameters
  PaStream* stream;           // PortAudio stream, provides access to audio hardware
  int paInitFailed;           // if Pa_Initialize() returns an error Pa_Terminate() should NOT be called
  AudioFeed* audioFeed;
  NSDF* pitch;                // pitch recognition

  Median *clarityMedian;
  Median *freqMedian;

  Buffer *audioBuffer;
  int peakSampleCount;
  int peakWindowSize;
  float peak;                 // audio peak - highest peak in peakWindowSize
  float tempPeak;             // temp peak for each audio frame (in audio callback)

  DetectionMode mode;
  Note currentNote;

  int strobeCount;
  Strobe* strobes[MAX_STROBES];
  Buffer *strobeBuffers[MAX_STROBES];

} Engine;




Engine* Engine_create(void);
void Engine_destroy(Engine* self);

int Engine_setInputDevice(Engine *self, int device, int samplerate, int bufferSize);
int Engine_readStrobe(Engine* self, int index);
int Engine_readStrobes(Engine* self);
float Engine_gain(Engine* self);
void Engine_estimatePitch(Engine* self);

void Engine_setStrobes(Engine* self, Note note, int samplerate);
void Engine_setCentsOffset(Engine* self, float cents);
void Engine_setPitchStandard(Engine* self, float freq);
void Engine_setGain(Engine* self, float gain);
void Engine_setColors(Engine* self, int colors[][3], int count);

void Engine_getSamplerates(int device, int outSamplerates[11]);
int Engine_getDefaultInputDevice(void);
int Engine_deviceCount(void);
const DeviceInfo* Engine_deviceInfo(int index);


#endif
