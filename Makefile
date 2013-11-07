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

DSP					= ../AudioPlayground/biquad/biquad.c -I../AudioPlayground/biquad \
						../AudioPlayground/src/Interpolator.c -I../AudioPlayground/src \
						-I../AudioPlayground/fir

GLFW			= -I/usr/local/includes/GL/glfw -lglfw

GL				= -framework OpenGL


FLAGS			= -Weverything -Wno-padded -Wno-unused-parameter -Wno-conversion
MAC_FLAGS	= -Wno-direct-ivar-access -Wno-objc-missing-property-synthesis -Wno-implicit-atomic-properties -Wno-auto-import

all:
	make mac

engine:
	cc -c $(FLAGS) -O3 -arch x86_64 $(FFTS) $(PORTAUDIO) $(DSP) src/engine/*.c
	ar rcs engine.a *.o
	rm *.o

mac:
	make engine
	cc $(FLAGS) $(MAC_FLAGS) -fmodules -arch x86_64 -fobjc-arc -O3 $(FFTS) $(PORTAUDIO) $(DSP) \
		-framework Cocoa -framework AppKit -framework QuartzCore -framework OpenGL \
		engine.a src/mac/*.m src/mac/*.c \
		-o Strobie.app/Contents/MacOS/strobie
		ibtool --compile Strobie.app/Contents/Resources/Application.nib src/mac/Application.xib
		cp src/mac/Info.plist Strobie.app/Contents/Info.plist
		# cp src/mac/Resources/*.png Strobie.app/Contents/Resources/




###########

test:
	make engine
	cc -Wall -O2 -Isrc/engine $(GL) $(GLFW) $(FFTS) $(PORTAUDIO) $(DSP) src/test.c engine.a -o test

experiment:
	cc -Wall $(FFTS) $(PORTAUDIO) src/experiment.c -o experiment

