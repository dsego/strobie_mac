/*
  Copyright (C) 2013 Davorin Šego
*/

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "config.h"

Config* Config_create()
{
  Config* cfg = malloc(sizeof(Config));
  assert(cfg != NULL);

  // general
  cfg->sample_rate                = 44100;
  cfg->buffer_length              = 512;
  cfg->resampled_buffer_length    = 512 * 25;

  // pitch recognition
  cfg->estimation_framerate = 20;
  cfg->fft_sample_rate      = 44100;
  cfg->fft_length           = 4096;
  cfg->audio_threshold      = -40;
  cfg->pitch_standard       = 440;
  cfg->display_flats        = false;
  cfg->transpose            = 0;
  cfg->cents_offset         = 0;

  // strobe display
  cfg->strobe_framerate  = 60;
  cfg->strobe_gain       = 1000;
  cfg->strobe_background = (RGB) { .r = 0.157, .g = 0.110, .b = 0.055 };
  cfg->strobe_foreground = (RGB) { .r = 0.984, .g = 0.627, .b = 0.106 };

  // strobe bands
  cfg->periods_per_frame     = 1;
  cfg->partials[0]           = 1;
  cfg->partials[1]           = 2;
  cfg->partials[2]           = 4;
  cfg->samples_per_period[0] = 64;
  cfg->samples_per_period[1] = 128;
  cfg->samples_per_period[2] = 256;

  // standard guitar tuning (in cents)
  cfg->ins_notes[0] = -2900;
  cfg->ins_notes[1] = -2400;
  cfg->ins_notes[2] = -1900;
  cfg->ins_notes[3] = -1400;
  cfg->ins_notes[4] = -1000;
  cfg->ins_notes[5] = -500;

  return cfg;
}


void Config_destroy(Config* cfg)
{
  free(cfg);
}