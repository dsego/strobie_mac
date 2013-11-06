/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/



#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "portaudio.h"




float level = 4;



int callback(
  const void* input,
  void* output,
  unsigned long frameCount,
  const PaStreamCallbackTimeInfo* time,
  PaStreamCallbackFlags status,
  void *userData
) {

  // nothing to process
  if (status == paInputUnderflow) { return paContinue; }

  float *samples = (float*) input;

  float sum = 0.0;
  for (int i = 0; i < frameCount; ++i) {
    sum += samples[i] * samples[i];
  }

  level =  sqrt(sum / (float)frameCount);

  return paContinue;

}



int main() {

  PaError err = Pa_Initialize();

  if (err) { return -1; }

  int device = 2;
  PaStreamParameters params;
  params.device = device;
  params.channelCount = 1;
  params.sampleFormat = paFloat32;
  params.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;
  params.hostApiSpecificStreamInfo = NULL;

  PaStream *stream;

  Pa_OpenStream(&stream, &params, NULL, 44100, paFramesPerBufferUnspecified, paNoFlag, &callback, NULL);
  Pa_StartStream(stream);

  while (1) {

    printf("%f            \r", level);
    fflush(stdout);

    usleep(100000);
  }

  Pa_Terminate();

  return 0;

}
