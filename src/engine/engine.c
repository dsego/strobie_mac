/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <math.h>
#include "Engine.h"
#include "utils.h"





Engine* Engine_create() {

  Engine* self = malloc(sizeof(Engine));
  assert(self != NULL);

  Config* config = self->config = Config_create();

  self->audioFeed = AudioFeed_create();
  AudioFeed_setDecimationRate(self->audioFeed, config->decimationRate);

  self->strobeCount = min(config->partialsLength, MAX_STROBES);


  // initialize strobes

  for (int i = 0; i < self->strobeCount; ++i) {

    self->strobes[i] = Strobe_create(
      config->bufferLength,
      config->resampledBufferLength,
      config->samplerate,
      config->samplesPerPeriod[i]
    );

    self->strobeBufferLengths[i] = config->samplesPerPeriod[i] *
      config->periodsPerFrame *
      config->partials[i];

    self->strobeBuffers[i] = calloc(self->strobeBufferLengths[i], sizeof(float));
    assert(self->strobeBuffers[i] != NULL);

  }
  self->pitch = Pitch_create(config->fftSamplerate, config->fftLength);

  self->pitchIndex = 0;
  memset(self->pitches, 0.0, sizeof(self->pitches));

  self->threshold = from_dBFS(self->config->audioThreshold);

  self->audioBuffer = malloc(self->config->fftLength * sizeof(float));
  assert(self->audioBuffer != NULL);

  return self;

}


void Engine_destroy(Engine* self) {

  Config_destroy(self->config);
  AudioFeed_destroy(self->audioFeed);
  Pitch_destroy(self->pitch);

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_destroy(self->strobes[i]);
    free(self->strobeBuffers[i]);
  }

  free(self->audioBuffer);
  free(self);
  self = NULL;

}


void Engine_processSignal(Engine* self, float* input, int length) {

  AudioFeed_process(self->audioFeed, input, length);

  // for (int i = 0; i < self->strobeCount; ++i) {
  //   Strobe_process(self->strobes[i], input, length);
  // }

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
  Engine* self = (Engine*) userData;

  int index  = 0;
  int length = (int) frameCount;

  // process in batches because frameCount can be bigger than bufferLength
  while (length > self->config->bufferLength) {

    Engine_processSignal(self, &finput[index], self->config->bufferLength);
    length -= self->config->bufferLength;
    index  += self->config->bufferLength;

  }

  // anything left over
  if (length > 0) {
    Engine_processSignal(self, &finput[index], length);
  }

  return paContinue;

}


void printPaError(PaError error) {

  printf("PortAudio error: %s\n", Pa_GetErrorText(error));

}


bool Engine_startAudio(Engine* self) {

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



  PaDeviceIndex count = Pa_GetDeviceCount();
  for (int i = 0; i < count; ++i) {

    const PaDeviceInfo* info = Pa_GetDeviceInfo(i);
    printf("%i: %s\n", i, info->name);

  }


  // err = Pa_OpenDefaultStream(
  //   &engine->stream,
  //   1, 0, paFloat32,
  //   engine->config->samplerate,
  //   paFramesPerBufferUnspecified,
  //   &Engine_streamCallback, engine
  // );

  int device = self->config->deviceIndex;
  // int deviceIndex = 0;

  const PaDeviceInfo* info = Pa_GetDeviceInfo(device);
  PaStreamParameters* inputParameters = malloc(sizeof(PaStreamParameters));

  inputParameters->device = device;
  inputParameters->channelCount = 1;
  inputParameters->sampleFormat = paFloat32;
  inputParameters->suggestedLatency = info->defaultLowInputLatency;
  inputParameters->hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
    &self->stream,
    inputParameters,
    NULL,
    self->config->samplerate,
    paFramesPerBufferUnspecified,
    paNoFlag,
    &Engine_streamCallback,
    self
  );

  if (err != paNoError) {
    printPaError(err);
    Pa_Terminate();
    return false;
  }

  err = Pa_StartStream(self->stream);

  if (err != paNoError) {
    printPaError(err);
    Pa_Terminate();
    return false;
  }


  // const PaStreamInfo* streamInfo = Pa_GetStreamInfo(self->stream);

  return true;

}


/*
  PortAudio docs say:
    Pa_Terminate() MUST be called before exiting a program which uses PortAudio.
    Failure to do so may result in serious resource leaks,
    such as audio devices not being available until the next reboot.
 */
void Engine_stopAudio(Engine* self) {

  Pa_StopStream(self->stream);

  // this will automatically close any PortAudio streams that are still open.
  Pa_Terminate();

}



void Engine_setStrobeFreq(Engine* self, double frequency) {

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_setFreq(self->strobes[i], frequency * self->config->partials[i]);
  }

}


void Engine_readStrobes(Engine* self) {

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_read(self->strobes[i], self->strobeBuffers[i], self->strobeBufferLengths[i]);
  }

}


double Engine_estimatePitch(Engine* self) {

  // double peak = findPeak(self->audioBuffer, self->config->fftLength);

  // if (peak > self->threshold) {


  // int dataLength = self->config->fftLength + self->config->hopSize;
  int length = self->config->fftLength;


  // read in new data from the ring buffer
  AudioFeed_read(self->audioFeed, self->audioBuffer, length);


  double pitch = Pitch_estimate(self->pitch, self->audioBuffer, length);


  // self->pitches[self->pitchIndex] = pitch;
  // self->pitchIndex += 1;

  // if (self->pitchIndex == self->config->averageCount) {
  //   self->pitchIndex = 0;
  // }

  // double sum = 0.0;
  // for (int i = 0; i < self->config->averageCount; ++i) {
  //   sum += self->pitches[i];
  // }

  // double avgPitch = sum / (double)self->config->averageCount;



    // pitch = clamp(pitch, 27.5000, 4186.01); // A0 - C8

  // }

  return pitch;

}