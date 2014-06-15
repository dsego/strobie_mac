// Copyright (c) Davorin Šego. All rights reserved.

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "engine.h"


#define ENGINE_STR_BUFFER_LENGTH 4096
#define min(a,b) ((a) < (b) ? (a) : (b))

static void printPaError(PaError err) {
  fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
}


Engine* Engine_create() {

  Engine* self = malloc(sizeof(Engine));
  assert(self != NULL);

  Config* config = self->config = Config_create();

  self->currentNote = EqualTemp_findNearest(
    self->config->freq,
    self->config->pitchStandard,
    self->config->centsOffset
  );

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
    self->strobeBuffers[i] = Buffer_create(ENGINE_STR_BUFFER_LENGTH, sizeof(float));
    self->strobeBuffers[i]->count = 0;

  }

  self->pitch = NSDF_create(config->samplerate, config->windowSize);
  self->audioBuffer = Buffer_create(self->config->windowSize, sizeof(float));
  self->peakSampleCount = 0;
  self->peakWindowSize = 4096; // circa 100ms
  self->tempPeak = 0;
  self->peak = 0;
  self->stream = NULL;
  self->paInitFailed = 0;

  //  ... if Pa_Initialize() returns an error code,
  //      Pa_Terminate() should NOT be called

  PaError err = Pa_Initialize();
  if (err != paNoError) {
    // TODO: how to handle error?
    printPaError(err);
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
  NSDF_destroy(self->pitch);

  for (int i = 0; i < self->strobeCount; ++i) {
    Strobe_destroy(self->strobes[i]);
    Buffer_destroy(self->strobeBuffers[i]);
  }

  Buffer_destroy(self->audioBuffer);
  free(self);
  self = NULL;

}


void Engine_setCentsOffset(Engine* self, float cents) {

  if (cents >= -50 && cents <= 50) {
    self->config->centsOffset = cents;
    self->currentNote = EqualTemp_centsToNote(
      self->currentNote.cents,
      self->config->pitchStandard,
      self->config->centsOffset
    );
    Engine_setStrobes(self, self->currentNote, self->config->samplerate);
  }

}


void Engine_setPitchStandard(Engine* self, float freq) {

  if (freq >= 100 && freq <= 1000) {
    self->config->pitchStandard = freq;
    self->currentNote = EqualTemp_centsToNote(
      self->currentNote.cents,
      self->config->pitchStandard,
      self->config->centsOffset
    );
    Engine_setStrobes(self, self->currentNote, self->config->samplerate);
  }

}


void Engine_setStrobes(Engine* self, Note note, int samplerate) {

  self->config->samplerate = samplerate;

  // reset octave
  if (note.octave > 8) {
    self->currentNote = EqualTemp_moveToOctave(note, 0);
  }
  else if (note.octave < 0) {
    self->currentNote = EqualTemp_moveToOctave(note, 8);
  }
  else {
    self->currentNote = note;
  }

  self->config->freq = self->currentNote.frequency;


  for (int i = 0; i < self->strobeCount; ++i) {

    if (self->config->strobes[i].mode == OCTAVE) {
      Note movedNote = EqualTemp_moveToOctave(
        self->currentNote,
        self->config->strobes[i].value
      );
      Strobe_setFreq(self->strobes[i], movedNote.frequency, self->config->samplerate);
    }
    else if (self->config->strobes[i].mode == PARTIAL) {
      float freq = self->currentNote.frequency * self->config->strobes[i].value;
      Strobe_setFreq(self->strobes[i], freq, self->config->samplerate );
    }
    else {
      // do nothing, strobe is already set to a particular note or frequency
    }

    // number of samples displayed
    self->strobeBuffers[i]->count = self->config->strobes[i].periodsPerFrame *
      self->config->strobes[i].samplesPerPeriod;

  }

}


float Engine_gain(Engine* self) {

  if (self->config->gain <= 0) {
    return 0;
  }
  float gain = self->config->gain;
  float peak = self->peak;

  // limit gain
  //  100 / (0 + 1/100) = 10000
  //  100 / (1 + 1/100) = 99
  return gain / (peak + 1.0 / gain);
  // return gain;

}


// float Engine_gain(Engine* self, float alpha) {

//   // 100 / (0 + 0.01) = 10000
//   // 100 / (1 + 0.01) = 99
//   float beta = alpha / self->config->maxGain; // limit gain
//   return alpha / (self->peak + beta);

// }




static inline float findWavePeak(const float* buffer, int bufferLength) {

  float peak = 0;

  for (int i = 0; i < bufferLength; ++i) {
    float pk = fabs(buffer[i]);
    if (pk > peak) {
      peak = pk;
    }
  }

  return peak;

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

  // find temp peak
  float peak = findWavePeak(samples, frameCount);
  if (peak > self->tempPeak) {
    self->tempPeak = peak;
  }
  self->peakSampleCount += frameCount;

  // full window reached
  if (self->peakSampleCount >= self->peakWindowSize) {
    self->peak = self->tempPeak;
    self->tempPeak = 0; // reset temp peak
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
  AudioFeed_read(
    self->audioFeed,
    (float*)self->audioBuffer->data,
    self->audioBuffer->count
  );

  float freq, clarity;
  NSDF_estimate(self->pitch, (float*)self->audioBuffer->data, &freq, &clarity);

  Median_push(self->freqMedian, freq);
  Median_push(self->clarityMedian, clarity);
  freq = Median_value(self->freqMedian);
  clarity = Median_value(self->clarityMedian);

  // totally not arbitrary
  const float maxFreq = 12345;

  if (self->mode == AUTO &&
      freq > 0.0 && freq < maxFreq &&
      clarity > self->config->clarityThreshold) {

    self->currentNote = EqualTemp_findNearest(
      freq,
      self->config->pitchStandard,
      self->config->centsOffset
    );
    Engine_setStrobes(self, self->currentNote, self->config->samplerate);

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
  if (currentDevice == device &&
    currentBufferSize == bufferSize &&
    currentSamplerate == samplerate) {
    return 0;
  }

  // if device index doesn't exist, use default device
  if (device < 0 || device >= Pa_GetDeviceCount()) {
    device = Pa_GetDefaultInputDevice();
    if (device == paNoDevice) {
      return 1;
    }
  }

  self->config->inputDevice = currentDevice = device;
  self->config->inputBufferSize = currentBufferSize = bufferSize;
  self->config->samplerate = currentSamplerate = samplerate;

  PaError err;

  if (self->stream != NULL) {
    Pa_AbortStream(self->stream);
    Pa_CloseStream(self->stream);
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

  if (err != paNoError) {
    printPaError(err);
    return 1;
  }

  err = Pa_StartStream(self->stream);

  if (err != paNoError) {
    printPaError(err);
    return 1;
  }

  return 0;

}


int Engine_getDefaultInputDevice() {

  return Pa_GetDefaultInputDevice();

}


int Engine_deviceCount() {

  return Pa_GetDeviceCount();

}


const DeviceInfo* Engine_deviceInfo(int index) {

  return Pa_GetDeviceInfo(index);

}
