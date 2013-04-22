//
//  Copyright (c) 2013 Davorin Šego. All rights reserved.
//

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define CONFIG_MAX_PARTIALS 10
#define CONFIG_MAX_NOTES    50

typedef struct {
  int sample_rate;
  int fft_sample_rate;
  int fft_length;
  int buffer_length;
  int resampled_buffer_length;
  int periods_per_frame;
  double audio_threshold;
  double pitch_standard;
  bool display_flats;
  int transpose;
  double cents_offset;
  int partials[CONFIG_MAX_PARTIALS];
  int samples_per_period[CONFIG_MAX_PARTIALS];
  double instrument_notes[CONFIG_MAX_NOTES];
} Config;


Config* Config_create();
void Config_destroy(Config* cfg);