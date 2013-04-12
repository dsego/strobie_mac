#
# Copyright (C) 2013 Davorin Šego
#


KISS_FFT  = -I../kiss_fft130/tools -I../kiss_fft130 ../kiss_fft130/kiss_fft.c ../kiss_fft130/tools/kiss_fftr.c
PORTAUDIO = ../portaudio/src/common/pa_ringbuffer.c -I../portaudio/include -I../portaudio/src/common -lportaudio

all:
	cc -c src/biquad.h src/biquad.c \
				src/tuning.c src/src.c src/pitch_estimation.c src/config.c src/strobe.c \
				$(KISS_FFT) $(PORTAUDIO)