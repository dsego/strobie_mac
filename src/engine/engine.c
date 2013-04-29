/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <math.h>
#include "Engine.h"
#include "utils.h"


Engine* Engine_create() {

  Engine* engine = malloc(sizeof(Engine));
  assert(engine != NULL);

  Config* config = engine->config = Config_create();

  engine->audioFeed = AudioFeed_create();

  engine->strobeCount = min(config->partialsLength, MAX_STROBES);


  // initialize strobes
  for (int i = 0; i < engine->strobeCount; ++i) {

    engine->strobes[i] = Strobe_create(
      config->bufferLength,
      config->resampledBufferLength,
      config->samplerate,
      config->samplesPerPeriod[i]
    );

    engine->strobeBufferLengths[i] = config->samplesPerPeriod[i] *
      config->periodsPerFrame *
      config->partials[i];

    engine->strobeBuffers[i] = calloc(engine->strobeBufferLengths[i], sizeof(float));
    assert(engine->strobeBuffers[i] != NULL);

  }

  // initialize pitch recognition

  engine->pitchEstimation = PitchEstimation_create(
    config->fftSamplerate,
    config->fftLength
  );

  engine->threshold = from_dBFS(engine->config->audioThreshold);

  engine->audioBuffer = calloc(engine->config->fftLength, sizeof(float));
  assert(engine->audioBuffer != NULL);

  return engine;

}


void Engine_destroy(Engine* engine) {

  Config_destroy(engine->config);
  AudioFeed_destroy(engine->audioFeed);
  PitchEstimation_destroy(engine->pitchEstimation);

  for (int i = 0; i < engine->strobeCount; ++i) {
    Strobe_destroy(engine->strobes[i]);
    free(engine->strobeBuffers[i]);
  }

  free(engine->audioBuffer);
  free(engine);
  engine = NULL;

}


void Engine_processSignal(Engine* engine, float* input, int inputLength) {

  AudioFeed_process(engine->audioFeed, input, inputLength);

  for (int i = 0; i < engine->strobeCount; ++i) {
    Strobe_process(engine->strobes[i], input, inputLength);
  }

}


// fetch audio data from the sound card and process

int Engine_streamCallback(
  const void* input,
  void* output,
  unsigned long frameCount,
  const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags,
  void *userData
) {

  // nothing to process
  if (statusFlags == paInputUnderflow) {
    return 0;
  }

  float* finput  = (float*) input;
  Engine* engine = (Engine*) userData;

  int index  = 0;
  int length = (int) frameCount;

  // process in batches because frameCount can be bigger than bufferLength
  while (length > engine->config->bufferLength) {

    Engine_processSignal(engine, &finput[index], engine->config->bufferLength);
    length -= engine->config->bufferLength;
    index  += engine->config->bufferLength;

  }

  // anything left over
  if (length > 0) {
    Engine_processSignal(engine, &finput[index], length);
  }

  return 0;

}


void printPaError(PaError error) {

  printf("PortAudio error: %s\n", Pa_GetErrorText(error));

}


bool Engine_startAudio(Engine* engine) {

  PaError err;

  // start PortAudio and open the input stream
  //  ... if Pa_Initialize() returns an error code,
  //      Pa_Terminate() should NOT be called
  //
  err = Pa_Initialize();
  if (err != paNoError) {
    printPaError(err);
    return false;
  }

  err = Pa_OpenDefaultStream(
    &engine->stream, 1, 0, paFloat32,
    engine->config->samplerate,
    paFramesPerBufferUnspecified,
    &Engine_streamCallback, engine
  );

  if (err != paNoError) {
    printPaError(err);
    Pa_Terminate();
    return false;
  }

  err = Pa_StartStream(engine->stream);

  if (err != paNoError) {
    printPaError(err);
    Pa_Terminate();
    return false;
  }

  return true;

}


/*
  PortAudio docs say:
    Pa_Terminate() MUST be called before exiting a program which uses PortAudio.
    Failure to do so may result in serious resource leaks,
    such as audio devices not being available until the next reboot.
 */
void Engine_stopAudio(Engine* engine) {

  Pa_StopStream(engine->stream);

  // this will automatically close any PortAudio streams that are still open.
  Pa_Terminate();

}



void Engine_setStrobeFreq(Engine* engine, double frequency) {

  for (int i = 0; i < engine->strobeCount; ++i) {
    Strobe_setFreq(engine->strobes[i], frequency * engine->config->partials[i]);
  }

}


void Engine_readStrobes(Engine* engine) {

  for (int i = 0; i < engine->strobeCount; ++i) {
    Strobe_read(
      engine->strobes[i],
      engine->strobeBuffers[i],
      engine->strobeBufferLengths[i]
    );
  }

}


double Engine_estimatePitch(Engine* engine) {

  static double pitch = 27.5000;

  // read in new data from the ring buffer
  AudioFeed_read(
    engine->audioFeed,
    engine->audioBuffer,
    engine->config->fftLength
  );

  // double peak = findPeak(engine->audioBuffer, engine->config->fftLength);

  // if (peak > engine->threshold) {

    pitch = PitchEstimation_fromAutocorrelation(
      engine->pitchEstimation,
      engine->audioBuffer,
      engine->config->fftLength
    );
    // pitch = clamp(pitch, 27.5000, 4186.01); // A0 - C8

  // }

  return pitch;

}