/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Engine.h"
#include "utils.h"


#define ENGINE_STR_BUFFER_LENGTH 4096






Engine* Engine_create() {

  Engine* self = malloc(sizeof(Engine));
  assert(self != NULL);

  Config* config = self->config = Config_create();

  self->currentNote = Tuning12TET_find(self->config->freq, self->config->pitchStandard, self->config->centsOffset);
  self->mode = MANUAL;
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

    // allocate enough space for any strobe size (see Engine_readStrobes)
    self->strobeBuffers[i] = FloatArray_create(ENGINE_STR_BUFFER_LENGTH);
    self->strobeLengths[i] = 0;

  }

  self->pitch = Pitch_create(config->samplerate, config->fftLength);
  self->audioBuffer = FloatArray_create(self->config->fftLength);
  self->level = 0;
  self->stream = NULL;
  self->paInitFailed = 0;

  //  ... if Pa_Initialize() returns an error code,
  //      Pa_Terminate() should NOT be called

  PaError err = Pa_Initialize();
  if (err != paNoError) {
    self->paInitFailed = 1;
  }

  return self;

}


void Engine_destroy(Engine* self) {

  Pa_CloseStream(self->stream);

  /*
    PortAudio docs say:
      Pa_Terminate() MUST be called before exiting a program which uses PortAudio.
      Failure to do so may result in serious resource leaks,
      such as audio devices not being available until the next reboot.
   */
  if (self->paInitFailed == 0) {
    Pa_Terminate();
  }

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


void Engine_setCentsOffset(Engine* self, float cents) {

  if (cents >= -50 && cents <= 50) {
    self->config->centsOffset = cents;
    self->currentNote = Tuning12TET_centsToNote(self->currentNote.cents, self->config->pitchStandard, self->config->centsOffset);
    Engine_setStrobes(self, self->currentNote);
  }

}


void Engine_setPitchStandard(Engine* self, float freq) {

  if (freq >= 100 && freq <= 1000) {
    self->config->pitchStandard = freq;
    self->currentNote = Tuning12TET_centsToNote(self->currentNote.cents, self->config->pitchStandard, self->config->centsOffset);
    Engine_setStrobes(self, self->currentNote);
  }

}


void Engine_setStrobes(Engine* self, Note note) {

  // reset octave
  if (note.octave > 8) {
    self->currentNote = Tuning12TET_moveToOctave(note, 0);
  }
  else if (note.octave < 0) {
    self->currentNote = Tuning12TET_moveToOctave(note, 8);
  }
  else {
    self->currentNote = note;
  }

  self->config->freq = self->currentNote.frequency;

  for (int i = 0; i < self->strobeCount; ++i) {

    if (self->config->strobes[i].mode == OCTAVE) {
      Note movedNote = Tuning12TET_moveToOctave(self->currentNote, self->config->strobes[i].value);
      Strobe_setFreq(self->strobes[i], movedNote.frequency);
    }
    else if (self->config->strobes[i].mode == PARTIAL) {
      Strobe_setFreq(self->strobes[i], self->currentNote.frequency * self->config->strobes[i].value);
    }
    else {
      // do nothing, strobe is already set to a particular note or frequency
    }

    // number of samples displayed
    self->strobeLengths[i] = self->config->strobes[i].periodsPerFrame * self->config->strobes[i].samplesPerPeriod;

  }

}


void Engine_readStrobes(Engine* self) {

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_read(self->strobes[i], self->strobeBuffers[i].elements, self->strobeLengths[i]);
  }

}




static inline void calcLevel(float* input, int length, float* level) {

  static int count = 0;
  static float sum = 0.0;

  for (int i = 0; i < length; ++i) {
    sum += input[i] * input[i];
    count++;
  }

  // should be around 100 ms
  if (count >= 4096) {
    *level = sqrt(sum / (float)count);
    sum = 0.0;
    count = 0;
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
  if (status == paInputUnderflow) { return paContinue; }

  Engine* self = (Engine*) userData;
  float *samples = (float*)input;

  AudioFeed_process(self->audioFeed, samples, frameCount);

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_process(self->strobes[i], samples, frameCount);
  }

  // calcLevel(samples, frameCount, &self->level);

  return paContinue;

}


float Engine_estimatePitch(Engine* self) {

  #define W_LENGTH 5
  static float window[W_LENGTH] = { 0 };
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

  #undef W_LENGTH

}


int Engine_setInputDevice(Engine *self, int device, int samplerate) {

  // illegal value
  static int currentDevice = -1;

  // already current device, don't bother changing
  if (currentDevice == device) { return 1; }

  // if device index doesn't exist, use default device
  if (device < 0 || device >= Pa_GetDeviceCount()) {
    device = Pa_GetDefaultInputDevice();
  }

  self->config->inputDevice = currentDevice = device;

  PaError err;

  if (self->stream != NULL) {
    err = Pa_AbortStream(self->stream);
    err = Pa_CloseStream(self->stream);
  }

  PaStreamParameters params;
  params.device = device;
  params.channelCount = 1;
  params.sampleFormat = paFloat32;
  params.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;
  params.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
    &self->stream,
    &params,
    NULL,
    samplerate,
    paFramesPerBufferUnspecified,
    paNoFlag,
    &Engine_streamCallback,
    self
  );

  // TODO
  //
  // try different sample rates
  // while (err == paInvalidSampleRate) { }


  if (err != paNoError) {
    Pa_Terminate();
    return 0;
  }

  err = Pa_StartStream(self->stream);

  if (err != paNoError) {
    Pa_Terminate();
    return 0;
  }

  return 1;

}


int Engine_getDefaultInputDevice() {

  return Pa_GetDefaultInputDevice();

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


void Engine_setGain(Engine* self, float gain) {

  for (int i = 0; i < self->strobeCount; ++i) {
    self->config->strobes[i].gain = gain;
  }

}


void Engine_setColors(Engine* self, int color1[3], int color2[3]) {

  for (int i = 0; i < self->strobeCount; ++i) {

    self->config->strobes[i].color1[0] = color1[0];
    self->config->strobes[i].color1[1] = color1[1];
    self->config->strobes[i].color1[2] = color1[2];

    self->config->strobes[i].color2[0] = color2[0];
    self->config->strobes[i].color2[1] = color2[1];
    self->config->strobes[i].color2[2] = color2[2];
  }

}
