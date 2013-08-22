#
# Copyright (C) 2013 Davorin Šego
#


KISS = -I../kiss_fft130/tools -I../kiss_fft130 ../kiss_fft130/kiss_fft.c \
						../kiss_fft130/tools/kiss_fftr.c


FFTS = -I/usr/local/include/ffts /usr/local/lib/libffts.a

#
# to build PortAudio:
#
# 	CFLAGS="-w -arch x86_64" ./configure  --enable-mac-universal=NO && make
#
PORTAUDIO = ../portaudio/src/common/pa_ringbuffer.c -I../portaudio/include \
						-I../portaudio/src/common ../portaudio/lib/.libs/libportaudio.dylib

DSP	= ../AudioPlayground/biquad/biquad.c -I../AudioPlayground/biquad \
			../AudioPlayground/src/Interpolator.c -I../AudioPlayground/src \
			-I../AudioPlayground/fir

GLFW = -I/usr/local/includes/GL/glfw -lglfw

GL = -framework OpenGL


all:
	make mac


engine:
	cc -c -Wall -O2 -arch x86_64 $(KISS) $(FFTS) $(PORTAUDIO) $(DSP) src/engine/*.c
	ar rcs engine.a *.o
	rm *.o


test:
	make engine
	cc -Wall -O2 -Isrc/engine $(GL) $(GLFW) $(KISS) $(FFTS) $(PORTAUDIO) $(DSP) src/test.c engine.a -o test


mac:
	make engine
	cc -Wall -arch x86_64 -fobjc-arc -O2 $(KISS) $(FFTS) $(PORTAUDIO) $(DSP) \
		-framework Cocoa -framework AppKit -framework QuartzCore -framework OpenGL \
		engine.a src/mac/*.m \
		-o Strobie.app/Contents/MacOS/strobie