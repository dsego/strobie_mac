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

  self->freqMedian = Median_create(9);
  self->clarityMedian = Median_create(9);

  // TODO: this should probably be in Config, maybe?
  self->mode = AUTO;
  self->audioFeed = AudioFeed_create();
  self->strobeCount = min(config->strobeCount, MAX_STROBES);

  // initialize strobes

  for (int i = 0; i < self->strobeCount; ++i) {

    self->strobes[i] = Strobe_create(
      self->config->strobes[i].bufferLength,
      self->config->strobes[i].resampledLength,
      self->config->strobes[i].samplesPerPeriod,
      self->config->strobes[i].subdivCount
    );

    // allocate enough space for any strobe size (see Engine_readStrobes)
    self->strobeBuffers[i] = Vec_create(ENGINE_STR_BUFFER_LENGTH, sizeof(float));
    self->strobeBuffers[i]->count = 0;

  }

  self->pitch = Pitch_create(NSDF_METHOD, config->samplerate, config->windowSize);
  self->audioBuffer = Vec_create(self->config->windowSize, sizeof(float));
  self->peakSampleCount = 0;
  self->peakWindowSize = 512;
  self->tempPeak = 0;
  self->peak = 0;
  self->clarity = 0;
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
  Median_destroy(self->freqMedian);
  Median_destroy(self->clarityMedian);
  AudioFeed_destroy(self->audioFeed);
  Pitch_destroy(self->pitch);

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_destroy(self->strobes[i]);
    Vec_destroy(self->strobeBuffers[i]);
  }

  Vec_destroy(self->audioBuffer);
  free(self);
  self = NULL;

}


void Engine_setCentsOffset(Engine* self, float cents) {

  if (cents >= -50 && cents <= 50) {
    self->config->centsOffset = cents;
    self->currentNote = Tuning12TET_centsToNote(self->currentNote.cents, self->config->pitchStandard, self->config->centsOffset);
    Engine_setStrobes(self, self->currentNote, self->config->samplerate);
  }

}


void Engine_setPitchStandard(Engine* self, float freq) {

  if (freq >= 100 && freq <= 1000) {
    self->config->pitchStandard = freq;
    self->currentNote = Tuning12TET_centsToNote(self->currentNote.cents, self->config->pitchStandard, self->config->centsOffset);
    Engine_setStrobes(self, self->currentNote, self->config->samplerate);
  }

}


void Engine_setStrobes(Engine* self, Note note, int samplerate) {

  self->config->samplerate = samplerate;

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
      Strobe_setFreq(self->strobes[i], movedNote.frequency, self->config->samplerate);
    }
    else if (self->config->strobes[i].mode == PARTIAL) {
      Strobe_setFreq(
        self->strobes[i],
        self->currentNote.frequency * self->config->strobes[i].value,
        self->config->samplerate
      );
    }
    else {
      // do nothing, strobe is already set to a particular note or frequency
    }

    // number of samples displayed
    self->strobeBuffers[i]->count = self->config->strobes[i].periodsPerFrame * self->config->strobes[i].samplesPerPeriod;

  }

}


// return 0 if there is no new data
int Engine_readStrobe(Engine* self, int index) {

  int hasData = Strobe_read(self->strobes[index], self->strobeBuffers[index]->elements, self->strobeBuffers[index]->count);
  return hasData;

}


int Engine_readStrobes(Engine* self) {

  int fresh = 0;
  for (int i = 0; i < self->strobeCount; ++i) {
    fresh += Strobe_read(self->strobes[i], self->strobeBuffers[i]->elements, self->strobeBuffers[i]->count);
  }
  return fresh; // there is new data

}


float Engine_gain(Engine* self) {

  // 1 / (0 + 0.00001) = 100000
  // 1 / (1 + 0.00001) = 0.9999 (~= 1)
  const float maxGain = 100000; //  gain limit
  return self->config->gain / (self->peak + 1.0 / maxGain);

}


// float Engine_gain(Engine* self, float alpha) {

//   // 100 / (0 + 0.01) = 10000
//   // 100 / (1 + 0.01) = 99
//   float beta = alpha / self->config->maxGain; // limit gain
//   return alpha / (self->peak + beta);

// }


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

  // find temp peak
  float peak = findWavePeak(samples, frameCount);
  if (peak > self->tempPeak) {
    self->tempPeak = peak;
  }
  self->peakSampleCount += frameCount;

  // full window reached
  if (self->peakSampleCount >= self->peakWindowSize) {
    self->peak = self->tempPeak;
    self->peakSampleCount = 0;
  }

  AudioFeed_process(self->audioFeed, samples, frameCount);

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_process(self->strobes[i], samples, frameCount);
  }

  return paContinue;

}


void Engine_estimatePitch(Engine* self) {

  // read in new data from the ring buffer

  AudioFeed_read(self->audioFeed, (float*)self->audioBuffer->elements, self->audioBuffer->count);

  float freq, clarity;
  Pitch_estimate(self->pitch, (float*)self->audioBuffer->elements, &freq, &clarity);

  Median_push(self->freqMedian, freq);
  Median_push(self->clarityMedian, clarity);
  freq = Median_value(self->freqMedian);
  clarity = Median_value(self->clarityMedian);

  // totally not arbitrary
  const float maxFreq = 12345;

  if (self->mode == AUTO) {
    if (freq > 0.0 && freq < maxFreq && clarity > self->config->clarityThreshold) {
      self->currentNote = Tuning12TET_find(freq, self->config->pitchStandard, self->config->centsOffset);
      Engine_setStrobes(self, self->currentNote, self->config->samplerate);
    }
  }


}


int Engine_setInputDevice(Engine *self, int device, int samplerate, int bufferSize) {

  if (bufferSize <= 0) {
    bufferSize = paFramesPerBufferUnspecified;
  }

  // illegal value
  static int currentDevice = -1;
  static int currentBufferSize = -1;
  static int currentSamplerate = -1;

  // already current device, don't bother changing
  if (currentDevice == device && currentBufferSize == bufferSize && currentSamplerate == samplerate) { return 1; }

  // if device index doesn't exist, use default device
  if (device < 0 || device >= Pa_GetDeviceCount()) {
    device = Pa_GetDefaultInputDevice();
  }

  self->config->inputDevice = currentDevice = device;
  self->config->inputBufferSize = currentBufferSize = bufferSize;
  self->config->samplerate = currentSamplerate = samplerate;

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

  // NOTE
  // Calls to open and close streams are definitely not thread safe on any platform.
  // This means that if you plan to open and close streams from more than one thread,
  // you will need to provide a locking mechanism to ensure that you don't
  // open or close streams in different threads at the same time.
  err = Pa_OpenStream(
    &self->stream,
    &params,
    NULL,
    samplerate,
    bufferSize,
    paNoFlag,
    &Engine_streamCallback,
    self
  );

  if (err != paNoError) return 1;

  err = Pa_StartStream(self->stream);

  if (err != paNoError) return 1;

  return 0;

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
