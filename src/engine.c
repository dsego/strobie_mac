/*
  Copyright (C) 2013 Davorin Šego
*/

#include <stdlib.h>
#include <math.h>
#include "engine.h"
#include "utils.h"

void Engine_init_strobes(Engine* engine);
void Engine_init_pitch_recognition(Engine* engine);


Engine* Engine_create()
{
  Engine* engine = malloc(sizeof(Engine));
  assert(engine != NULL);

  Config* config = engine->config = Config_create();

  // frame rates
  double estimation_framerate = clamp(config->estimation_framerate, 1, 100);
  double strobe_framerate     = clamp(config->strobe_framerate, 1, 100);

  // sleep in microseconds
  engine->estimation_delay = 1000000 / estimation_framerate;
  engine->strobe_delay     = 1000000 / strobe_framerate;

  // initialize strobes
  Engine_init_strobes(engine);

  // pitch recognition
  Engine_init_pitch_recognition(engine);

  // to convert stream from float* to double*
  engine->stream_buffer = calloc(4096, sizeof(double));
  assert(engine->stream_buffer != NULL);

  return engine;
}

void Engine_init_pitch_recognition(Engine* engine)
{
  Config* config = engine->config;
  engine->pitch_estimation = PitchEstimation_create(config->fft_sample_rate, config->fft_length);
  engine->threshold = from_dbfs(engine->config->audio_threshold);
  engine->audio_buffer = calloc(engine->config->fft_length, sizeof(double));
                         assert(engine->audio_buffer != NULL);
  engine->ringbuffer_data = malloc(32768 * sizeof(double));
                            assert(engine->ringbuffer_data != NULL);
  engine->ringbuffer = malloc(sizeof(PaUtilRingBuffer));
                       assert(engine->ringbuffer != NULL);
  PaUtil_InitializeRingBuffer(engine->ringbuffer, sizeof(double), 32768, engine->ringbuffer_data);
}

void Engine_init_strobes(Engine* engine)
{
  Config* config = engine->config;
  int i = 0;
  while (i < CONFIG_MAX_PARTIALS && i < MAX_STROBES && config->partials[i] > 0 && config->samples_per_period[i] > 0) {
    engine->strobes[i] = Strobe_create(config->buffer_length,
                                       config->resampled_buffer_length,
                                       config->sample_rate,
                                       config->samples_per_period[i]);
    engine->strobe_buffer_lengths[i] = config->samples_per_period[i] * config->periods_per_frame * config->partials[i];
    engine->strobe_buffers[i] = calloc(engine->strobe_buffer_lengths[i], sizeof(double));
    assert(engine->strobe_buffers[i] != NULL);
    i = i + 1;
  }
  engine->strobe_count = i - 1;
}

void Engine_destroy(Engine* engine)
{
  Config_destroy(engine->config);
  PitchEstimation_destroy(engine->pitch_estimation);
  for (int i = 0; i < engine->strobe_count; ++i) {
    Strobe_destroy(engine->strobes[i]);
    free(engine->strobe_buffers[i]);
  }
  free(engine->audio_buffer);
  Pa_Terminate();
  free(engine);
}








void Engine_process_signal(Engine* engine, double* input, int input_len) {
  // converter.process_signal(input);
  for (int i = 0; i < engine->strobe_count; ++i) {
    Strobe_process(engine->strobes[i], input, input_len);
  }
}


// fetch audio data from the sound card and process
int Engine_stream_callback(const void* input, void* output, unsigned long nframes,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags, void *userData)
{

  float* finput = (float*) input;
  Engine* engine = (Engine*) userData;

  // convert to double
  for (int i = 0; i < nframes; ++i) {
    engine->stream_buffer[i] = (double) finput[i];
  }

  int index = 0;
  int length = (int) nframes;

  // process in batches because nframes can be bigger than config->buffer_length
  while (nframes > engine->config->buffer_length) {
    Engine_process_signal(engine, &engine->stream_buffer[index], engine->config->buffer_length);
    length -= engine->config->buffer_length;
    index += engine->config->buffer_length;
  }

  // anything left over
  if (length > 0) {
    Engine_process_signal(engine, &engine->stream_buffer[index], length);
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
  double peak = find_peak(engine->audio_buffer, engine->config->fft_length);
  if (peak > engine->threshold) {
    pitch = PitchEstimation_pitch_from_autocorrelation(engine->pitch_estimation,
                                                       engine->audio_buffer,
                                                       engine->config->fft_length);
    pitch = clamp(pitch, 27.5000, 4186.01); // A0 - C8
  }
  return pitch;
}



//   public void draw() {
//     window_background();

//     if (instrument_mode == true)
//       note = Tuning.12TET.find_nearest(pitch, config.ins_notes, config.pitch_standard);
//     else
//       note = Tuning.12TET.find(pitch, config.pitch_standard, config.cents_offset, config.transpose);

//     if (config.display_flats)
//       render_note(note.alt_letter, note.alt_sign, note.octave.to_string());
//     else
//       render_note(note.letter, note.sign, note.octave.to_string());

//     // float h = height / strobe_signals.length;

//     lock (strobe_signals) {
//       strobe_display(strobe_signals, config.strobe_gain, config.strobe_background, config.strobe_foreground);
//     }


//     // to_dbfs(peak);

//     // draw_signal(pitch_estimation.autocorr_data, 0.000005f);

//     // /* center */
//     // context.translate(width / 2, height / 2);

//     // /* keep aspect ratio */
//     // if (height > width)
//     //   context.scale(width, width);
//     // else
//     //   context.scale(height, height);

//     // draw_wheel(strobe_signals[2].data, 2000f);
//     // context.restore();
//   }




//   // protected process_events() {
//   //   base.process_events();

//   //   if (manual_mode == true) {
//   //     if (get_key(Key.UP)) {
//   //       pitch =
//   //     }
//   //   }
//   // }

//   public static int main(string[] args) {
//     if (!Thread.supported()) {
//       stderr.printf ("Cannot run without thread support.\n");
//       return 1;
//     }
//     var app = new App();
//     Thread<int> thread_a = new Thread<int>("a", app.find_pitch);
//     Thread<int> thread_b = new Thread<int>("b", app.strobe);

//     while (!app.quit) {
//       app.draw();
//       app.process_events();
//       Thread.usleep(10000);
//     }
//     return 0;
//   }

// }