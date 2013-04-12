#
# Copyright (C) 2013 Davorin Šego
#

# LIBRARIES
KISS_FFT  = -I../kiss_fft130/tools -I../kiss_fft130 ../kiss_fft130/kiss_fft.c ../kiss_fft130/tools/kiss_fftr.c
PORTAUDIO = ../portaudio/src/common/pa_ringbuffer.c -I../portaudio/include -I../portaudio/src/common -lportaudio

# COMPONENTS
BIQUAD    = src/biquad.h src/biquad.c
SRC       = src/src.h src/src.c
TUNING    = src/tuning.h src/tuning.c
PITCH     = src/pitch_estimation.h src/pitch_estimation.c
CONFIG    = src/config.h src/config.c

all:
	cc -c $(CONFIG) $(BIQUAD) $(SRC) $(TUNING) $(PITCH) \
				$(KISS_FFT) $(PORTAUDIO)