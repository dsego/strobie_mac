#
# Copyright (C) 2013 Davorin Šego
#

# LIBRARIES
KISS_FFT  = -I../kiss_fft130/tools -I../kiss_fft130 ../kiss_fft130/kiss_fft.c ../kiss_fft130/tools/kiss_fftr.c
PORTAUDIO = ../portaudio/src/common/pa_ringbuffer.c -I../portaudio/include -I../portaudio/src/common


GLFW      = -I/usr/local/includes/GL/glfw -lglfw


engine:
	cc -c -O2 $(KISS_FFT) $(PORTAUDIO) src/engine/*.c
	ar rcs engine.a *.o
	rm *.o

test:
	make engine
	cc $(KISS_FFT) $(PORTAUDIO) $(GLFW) src/engine/main.c engine.a -lportaudio -framework opengl -o test

mac:
	make engine
	cc -fobjc-arc -O2 $(KISS_FFT) $(PORTAUDIO) -framework Cocoa -framework AppKit engine.a -lportaudio src/mac/*.m \
		 -o Strobie.app/Contents/MacOS/strobie
