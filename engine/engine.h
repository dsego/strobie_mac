/*
  Copyright (C) 2013 Davorin Šego
*/

#include <stdbool.h>
#include "portaudio.h"
#include "tuning.h"
#include "config.h"
#include "pitch_estimation.h"
#include "audio_feed.h"
#include "strobe.h"

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
  double estimation_delay;
  double strobe_delay;
  double threshold;
  PaUtilRingBuffer* ringbuffer;
  float* ringbuffer_data;
} Engine;


Engine* Engine_create();
void Engine_destroy(Engine* engine);
bool Engine_init_audio(Engine* engine);
void Engine_read_strobes(Engine* engine, Note note);
double Engine_estimate_pitch(Engine* engine);