
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "rgb.h"

static const int CFG_MAX_PARTIALS = 10;
static const int CFG_MAX_MOTES = 50;

typedef struct {
  int sample_rate;
  int fft_sample_rate;
  int fft_length;
  int buffer_length;
  int resampled_buffer_length;
  int periods_per_frame;
  int estimation_framerate;
  int strobe_framerate;
  double audio_threshold;
  double strobe_gain;
  double pitch_standard;
  bool display_flats;
  int transpose;
  double cents_offset;
  int partials[CFG_MAX_PARTIALS];
  int samples_per_period[CFG_MAX_PARTIALS];
  double ins_notes[CFG_MAX_MOTES];
  RGB strobe_background;
  RGB strobe_foreground;
} Config;


Config* Config_create();
void Config_destroy(Config* cfg);