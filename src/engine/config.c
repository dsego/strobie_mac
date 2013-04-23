//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "Config.h"

Config* Config_create()
{
  Config* cfg = malloc(sizeof(Config));
  assert(cfg != NULL);

  // general
  cfg->samplerate             = 44100;
  cfg->bufferLength           = 512;
  cfg->resampledBufferLength  = 512 * 25;

  // pitch recognition
  cfg->fftSamplerate       = 44100;
  cfg->fftLength           = 4096;
  cfg->audioThreshold      = -60;
  cfg->pitchStandard       = 440;
  cfg->displayFlats        = false;
  cfg->transpose           = 0;
  cfg->centsOffset         = 0;

  // strobe bands
  cfg->periodsPerFrame     = 1;
  cfg->partialsLength      = 3;
  cfg->partials[0]         = 1;
  cfg->partials[1]         = 2;
  cfg->partials[2]         = 4;
  cfg->samplesPerPeriod[0] = 64;
  cfg->samplesPerPeriod[1] = 128;
  cfg->samplesPerPeriod[2] = 256;

  // standard guitar tuning (in cents)
  cfg->instrumentNotes[0] = -2900;
  cfg->instrumentNotes[1] = -2400;
  cfg->instrumentNotes[2] = -1900;
  cfg->instrumentNotes[3] = -1400;
  cfg->instrumentNotes[4] = -1000;
  cfg->instrumentNotes[5] = -500;

  return cfg;
}


void Config_destroy(Config* cfg)
{
  free(cfg);
  cfg = NULL;
}