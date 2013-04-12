#
# Copyright (C) 2013 Davorin Šego
#


KISS_FFT = -I../kiss_fft130/tools -I../kiss_fft130 ../kiss_fft130/kiss_fft.c ../kiss_fft130/tools/kiss_fftr.c

all:
	cc -c src/biquad.c src/tuning.c src/src.c src/pitch_estimation.c \
	$(KISS_FFT)