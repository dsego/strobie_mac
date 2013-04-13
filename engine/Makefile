#
# Copyright (C) 2013 Davorin Šego
#

# LIBRARIES
KISS_FFT  = -I../../kiss_fft130/tools -I../../kiss_fft130 ../../kiss_fft130/kiss_fft.c ../../kiss_fft130/tools/kiss_fftr.c
PORTAUDIO = ../../portaudio/src/common/pa_ringbuffer.c -I../../portaudio/include -I../../portaudio/src/common

all:
	cc -c $(KISS_FFT) $(PORTAUDIO) *.c
	ar rcs libengine.a *.o
	rm *.o