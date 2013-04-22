//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

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
  AudioFeed* audio_feed;
  PitchEstimation* pitch_estimation;
  Strobe* strobes[MAX_STROBES];
  int strobe_count;
  float* strobe_buffers[MAX_STROBES];
  int strobe_buffer_lengths[MAX_STROBES];
  float* audio_buffer;
  float* conversion_buffer;
  PaStream* stream;
  double threshold;
  PaUtilRingBuffer* ringbuffer;
  float* ringbuffer_data;
} Engine;


Engine* Engine_create();
void Engine_destroy(Engine* engine);

bool Engine_init_audio(Engine* engine);
void Engine_read_strobes(Engine* engine);
void Engine_set_strobe_freq(Engine* engine, double frequency);
double Engine_estimate_pitch(Engine* engine);