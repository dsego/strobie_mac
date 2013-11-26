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

GLFW			= -lglfw3

GL				= -framework OpenGL


WARN			= -Weverything -Wno-padded -Wno-unused-parameter -Wno-conversion
MAC_WARN	= $(WARN) -Wno-direct-ivar-access -Wno-objc-missing-property-synthesis -Wno-implicit-atomic-properties -Wno-auto-import


# -Ofast enables -O3, vectorization, strict aliasing,fast math
# -flto  is a link-time optimizer
# -ffast-math (?)
#  -fvectorize should be enabled by default for -O3
OPTIONS			= -O3 -arch x86_64 -flto
MAC_OPTIONS = $(OPTIONS) -fmodules -fobjc-arc


all:
	make mac

engine:
	cc -c $(WARN) $(OPTIONS) $(FFTS) $(PORTAUDIO) $(DSP) src/engine/*.c
	ar rcs engine.a *.o
	rm *.o

mac:
	make engine
	cc $(MAC_WARN) $(MAC_OPTIONS) $(FFTS) $(PORTAUDIO) $(DSP) \
		-framework Cocoa -framework AppKit -framework QuartzCore -framework OpenGL \
		engine.a src/mac/*.m src/mac/*.c \
		-o Strobie.app/Contents/MacOS/Strobie
		ibtool --compile Strobie.app/Contents/Resources/Application.nib src/mac/Application.xib
		cp src/mac/Info.plist Strobie.app/Contents/Info.plist
		# iconutil -c icns src/app.iconset
		# cp src/app.icns Strobie.app/Contents/Resources/app.icns



###########

.PHONY: test
test:
	make engine
	cc $(OPTIONS) $(WARN) -Isrc/engine $(GL) $(GLFW) $(FFTS) $(PORTAUDIO) $(DSP) src/test.c engine.a -o test

experiment:
	cc $(OPTIONS) $(WARN) $(FFTS) $(PORTAUDIO) src/experiment.c -o experiment

