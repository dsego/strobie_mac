#
# Copyright (C) 2013 Davorin Šego
#

# LIBRARIES
KISS_FFT  = -I../kiss_fft130/tools -I../kiss_fft130 ../kiss_fft130/kiss_fft.c ../kiss_fft130/tools/kiss_fftr.c
PORTAUDIO = ../portaudio/src/common/pa_ringbuffer.c -I../portaudio/include -I../portaudio/src/common
GLFW      = -I/usr/local/includes/GL/glfw -lglfw
DSP 			= ../AudioPlayground/biquad/biquad.c -I../AudioPlayground/biquad \
						../AudioPlayground/src/Interpolator.c -I../AudioPlayground/src \
						-I../AudioPlayground/fir ../AudioPlayground/fir/windows.c

engine:
	cc -c -g -Wall -O2 $(KISS_FFT) $(PORTAUDIO) $(DSP) src/engine/*.c
	ar rcs engine.a *.o
	rm *.o

mac:
	make engine
	cc -g -Wall -fobjc-arc -O2 $(KISS_FFT) $(PORTAUDIO) $(DSP) \
		-framework Cocoa -framework AppKit -framework QuartzCore -framework OpenGL \
		engine.a -lportaudio src/mac/*.m \
		-o Strobie.app/Contents/MacOS/strobie