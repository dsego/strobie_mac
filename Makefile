#
# Copyright (C) 2013 Davorin Šego
#

KISS_FFT  = -I../kiss_fft130/tools -I../kiss_fft130 ../kiss_fft130/kiss_fft.c \
						../kiss_fft130/tools/kiss_fftr.c

# to build PortAudio:
#
# 	CFLAGS="-w -arch x86_64" ./configure  --enable-mac-universal=NO && make
#
PORTAUDIO = ../portaudio/src/common/pa_ringbuffer.c -I../portaudio/include \
						-I../portaudio/src/common ../portaudio/lib/.libs/libportaudio.dylib

DSP 			= ../AudioPlayground/biquad/biquad.c -I../AudioPlayground/biquad \
						../AudioPlayground/src/Interpolator.c -I../AudioPlayground/src \
						-I../AudioPlayground/fir ../AudioPlayground/fir/windows.c

all:
	make mac


engine:
	cc -c -Wall -O2 -arch x86_64 $(KISS_FFT) $(PORTAUDIO) $(DSP) src/engine/*.c
	ar rcs engine.a *.o
	rm *.o


mac:
	make engine
	cc -Wall -arch x86_64 -fobjc-arc -O2 $(KISS_FFT) $(PORTAUDIO) $(DSP) \
		-framework Cocoa -framework AppKit -framework QuartzCore -framework OpenGL \
		engine.a src/mac/*.m \
		-o Strobie.app/Contents/MacOS/strobie