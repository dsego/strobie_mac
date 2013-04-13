/*
  Copyright (C) 2013 Davorin Šego
*/

#include <stdbool.h>
#include "portaudio.h"
#include "tuning.h"
#include "config.h"
#include "pitch_estimation.h"
#include "strobe.h"

#define MAX_STROBES 10

typedef struct {
  Config* config;
  PitchEstimation* pitch_estimation;
  Strobe* strobes[MAX_STROBES];
  int strobe_count;
  double* strobe_buffers[MAX_STROBES];
  int strobe_buffer_lengths[MAX_STROBES];
  double* audio_buffer;
  double* stream_buffer;
  PaStream* stream;
  double estimation_delay;
  double strobe_delay;
  double threshold;
  PaUtilRingBuffer* ringbuffer;
  double* ringbuffer_data;
} Engine;


Engine* Engine_create();
void Engine_destroy(Engine* engine);
bool Engine_init_audio(Engine* engine);
void Engine_read_strobes(Engine* engine, Note note);
double Engine_estimate_pitch(Engine* engine);