//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <stdlib.h>
#include <math.h>
#include "Engine.h"
#include "utils.h"


Engine* Engine_create()
{
  Engine* engine = malloc(sizeof(Engine));
  assert(engine != NULL);

  Config* config = engine->config = Config_create();
  engine->audio_feed = AudioFeed_create();

  // frame rates
  double estimation_framerate = clamp(config->estimation_framerate, 1, 100);
  double strobe_framerate     = clamp(config->strobe_framerate, 1, 100);

  // sleep in microseconds
  engine->estimation_delay = 1000000 / estimation_framerate;
  engine->strobe_delay     = 1000000 / strobe_framerate;


  // initialize strobes
  int i = 0;
  while (i < CONFIG_MAX_PARTIALS && i < MAX_STROBES && config->partials[i] > 0 && config->samples_per_period[i] > 0) {
    engine->strobes[i] = Strobe_create(config->buffer_length,
                                       config->resampled_buffer_length,
                                       config->sample_rate,
                                       config->samples_per_period[i]);
    engine->strobe_buffer_lengths[i] = config->samples_per_period[i] * config->periods_per_frame * config->partials[i];
    engine->strobe_buffers[i] = calloc(engine->strobe_buffer_lengths[i], sizeof(float));
    assert(engine->strobe_buffers[i] != NULL);
    i = i + 1;
  }
  engine->strobe_count = i - 1;


  // initialize pitch recognition
  engine->pitch_estimation = PitchEstimation_create(config->fft_sample_rate, config->fft_length);
  engine->threshold = from_dbfs(engine->config->audio_threshold);
  engine->audio_buffer = calloc(engine->config->fft_length, sizeof(float));
                         assert(engine->audio_buffer != NULL);
  engine->ringbuffer_data = malloc(32768 * sizeof(float));
                            assert(engine->ringbuffer_data != NULL);
  engine->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
                       assert(engine->ringbuffer != NULL);
  PaUtil_InitializeRingBuffer(engine->ringbuffer, sizeof(float), 32768, engine->ringbuffer_data);

  // to convert stream from float* to double*
  engine->conversion_buffer = malloc(config->buffer_length * sizeof(float));
  assert(engine->conversion_buffer != NULL);

  return engine;
}

void Engine_destroy(Engine* engine)
{
  Pa_Terminate();
  Config_destroy(engine->config);
  AudioFeed_destroy(engine->audio_feed);
  PitchEstimation_destroy(engine->pitch_estimation);
  for (int i = 0; i < engine->strobe_count; ++i) {
    Strobe_destroy(engine->strobes[i]);
    free(engine->strobe_buffers[i]);
  }
  free(engine->audio_buffer);
  free(engine);
}








void Engine_process_signal(Engine* engine, float* input, int input_len) {
  // convert to double
  for (int i = 0; i < input_len; ++i) {
    engine->conversion_buffer[i] = (double) input[i];
  }

  AudioFeed_process(engine->audio_feed, engine->conversion_buffer, input_len);
  for (int i = 0; i < engine->strobe_count; ++i) {
    Strobe_process(engine->strobes[i], engine->conversion_buffer, input_len);
  }
}


// fetch audio data from the sound card and process
int Engine_stream_callback(const void* input, void* output, unsigned long nframes,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags, void *userData)
{
  float* finput  = (float*) input;
  Engine* engine = (Engine*) userData;

  int index  = 0;
  int length = (int) nframes;

  // process in batches because nframes can be bigger than config->buffer_length
  while (nframes > engine->config->buffer_length) {
    Engine_process_signal(engine, &finput[index], engine->config->buffer_length);
    length -= engine->config->buffer_length;
    index  += engine->config->buffer_length;
  }

  // anything left over
  if (length > 0) {
    Engine_process_signal(engine, &finput[index], length);
  }

  return 0;
}



bool Engine_init_audio(Engine* engine)
{
  PaError err;

  // start PortAudio and open the input stream
  err = Pa_Initialize();
  if (err != paNoError)
    return false;

  err = Pa_OpenDefaultStream(&engine->stream, 1, 0, paFloat32,
                             engine->config->sample_rate,
                             paFramesPerBufferUnspecified,
                             &Engine_stream_callback, engine);
  if (err != paNoError)
    return false;

  err = Pa_StartStream(engine->stream);
  if (err != paNoError)
    return false;

  return true;
}







void Engine_read_strobes(Engine* engine, Note note)
{
  for (int i = 0; i < engine->strobe_count; ++i) {
    Strobe_read(engine->strobes[i], engine->strobe_buffers[i], engine->strobe_buffer_lengths[i]);
    Strobe_set_freq(engine->strobes[i], note.frequency * engine->config->partials[i]);
  }
}

double Engine_estimate_pitch(Engine* engine)
{
  static double pitch = 27.5000;

  // read in new data from the ring buffer
  AudioFeed_read(engine->audio_feed, engine->audio_buffer, engine->config->fft_length);
  double peak = find_peak(engine->audio_buffer, engine->config->fft_length);
  if (peak > engine->threshold) {
    pitch = PitchEstimation_pitch_from_autocorrelation(engine->pitch_estimation,
                                                       engine->audio_buffer,
                                                       engine->config->fft_length);
    pitch = clamp(pitch, 27.5000, 4186.01); // A0 - C8
  }
  return pitch;
}