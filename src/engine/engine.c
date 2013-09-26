/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <math.h>
#include "Engine.h"
#include "utils.h"



void Engine_resetStrobeBuffers(Engine* self);



Engine* Engine_create() {

  Engine* self = malloc(sizeof(Engine));
  assert(self != NULL);

  Config* config = self->config = Config_create();

  self->audioFeed = AudioFeed_create();
  self->strobeCount = min(config->strobeCount, MAX_STROBES);

  // initialize strobes

  for (int i = 0; i < self->strobeCount; ++i) {

    self->strobes[i] = Strobe_create(
      self->config->strobes[i].bufferLength,
      self->config->strobes[i].resampledLength,
      self->config->samplerate,
      self->config->strobes[i].samplesPerPeriod,
      self->config->strobes[i].subdivCount
    );

    self->strobeBuffers[i] = FloatArray_create(self->config->strobes[i].periodsPerFrame * self->config->strobes[i].samplesPerPeriod);

  }

  Engine_setStrobes(self, config->reference);
  // Strobe_setFreq(self->strobes[0], self->config->strobes[0].value);
  // Strobe_setFreq(self->strobes[1], self->config->strobes[1].value);

  self->pitch = Pitch_create(config->samplerate, config->fftLength);
  self->threshold = fromDecibel(self->config->audioThreshold);
  self->audioBuffer = FloatArray_create(self->config->fftLength);

  return self;

}


void Engine_destroy(Engine* self) {

  Config_destroy(self->config);
  AudioFeed_destroy(self->audioFeed);
  Pitch_destroy(self->pitch);

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_destroy(self->strobes[i]);
    FloatArray_destroy(self->strobeBuffers[i]);
  }

  FloatArray_destroy(self->audioBuffer);
  free(self);
  self = NULL;

}


void Engine_resetStrobeBuffers(Engine* self) {

  for (int i = 0; i < self->strobeCount; ++i) {

    if (self->strobeBuffers[i].elements != NULL) {
      FloatArray_destroy(self->strobeBuffers[i]);
    }

    self->strobeBuffers[i] = FloatArray_create(self->config->strobes[i].periodsPerFrame * self->config->strobes[i].samplesPerPeriod);

  }

}


void Engine_setStrobes(Engine* self, Note note) {

  for (int i = 0; i < self->strobeCount; ++i) {

    if (self->config->strobes[i].mode == OCTAVE) {
      Note movedNote = Tuning12TET_moveToOctave(note, self->config->strobes[i].value);
      Strobe_setFreq(self->strobes[i], movedNote.frequency);
    }
    else if (self->config->strobes[i].mode == PARTIAL) {
      Strobe_setFreq(self->strobes[i], note.frequency * self->config->strobes[i].value);
    }
    else {
      // do nothing, strobe is already set to a particular note or frequency
    }

  }

}


void Engine_readStrobes(Engine* self) {

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_read(self->strobes[i], self->strobeBuffers[i].elements, self->strobeBuffers[i].length);
  }

}









// fetch audio data from the sound card and process

static inline int Engine_streamCallback(
  const void* input,
  void* output,
  unsigned long frameCount,
  const PaStreamCallbackTimeInfo* time,
  PaStreamCallbackFlags status,
  void *userData
) {

  // nothing to process
  if (status == paInputUnderflow) { return 0; }

  Engine* self = (Engine*) userData;

  AudioFeed_process(self->audioFeed, (float*)input, frameCount);

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_process(self->strobes[i], (float*)input, frameCount);
  }

  return paContinue;

}


static inline void printPaError(PaError error) {

  printf("PortAudio error: %s\n", Pa_GetErrorText(error));

}


float Engine_estimatePitch(Engine* self) {

  // float peak = findPeak(self->audioBuffer, self->config->fftLength);

  // if (peak > self->threshold) {
  const int W_LENGTH = 5;
  static float window[W_LENGTH];
  static int index = 0;


  // read in new data from the ring buffer
  AudioFeed_read(self->audioFeed, self->audioBuffer.elements, self->audioBuffer.length);

  float pitch = Pitch_estimate(self->pitch, self->audioBuffer.elements);

  // rolling median filter
  window[index] = pitch;
  index += 1;
  if (index >= W_LENGTH) {
    index = 0;
  }

  return median5(window);

}








int Engine_startAudio(Engine* self) {

  PaError err;

  // start PortAudio and open the input stream
  //  ... if Pa_Initialize() returns an error code,
  //      Pa_Terminate() should NOT be called

  err = Pa_Initialize();
  if (err != paNoError) {
    printPaError(err);
    return 0;
  }

  const PaDeviceInfo* info = Pa_GetDeviceInfo(self->config->inputDevice);
  PaStreamParameters* inputParameters = malloc(sizeof(PaStreamParameters));

  inputParameters->device = self->config->inputDevice;
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
    return 0;
  }

  err = Pa_StartStream(self->stream);

  if (err != paNoError) {
    printPaError(err);
    Pa_Terminate();
    return 0;
  }

  return 1;

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


int Engine_deviceCount() {

  return Pa_GetDeviceCount();

}


int Engine_deviceName(int index, char *outName, int *outIsInput, int *outIsOutput) {

  const PaDeviceInfo* info = Pa_GetDeviceInfo(index);

  if (info == NULL) { return 0; }

  strcpy(outName, info->name);
  *outIsInput = (info->maxInputChannels > 0) ? 1 : 0;
  *outIsOutput = (info->maxOutputChannels > 0) ? 1 : 0;
  return 1;

}
