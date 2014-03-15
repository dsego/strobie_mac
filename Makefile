# Copyright (C) Davorin Šego

FFTS = -I/usr/local/include/ffts /usr/local/lib/libffts.a

# to build PortAudio: CFLAGS="-w -arch x86_64" ./configure  --enable-mac-universal=NO && make
PORTAUDIO = ../portaudio/trunk/src/common/pa_ringbuffer.c \
					-I../portaudio/trunk/include \
					-I../portaudio/trunk/src/common \
						../portaudio/trunk/lib/.libs/libportaudio.dylib

DSP      = ../Biquad/src/biquad.c -I../Biquad/src	../Interpolator/src/Intp.c -I../Interpolator/src
VEC      = ../Buffer/src/Buffer.c -I../Buffer/src
PITCH    = ../Pitch/src/NSDF.c -I../Pitch/src
TUNING   = ../Tuning/src/EqualTemp.c -I../Tuning/src
SHADER   = ../shader/src/shader.c -I../shader/src
MEDIAN   = ../median/src/Median.c -I../median/src
GLFW     = -lglfw3
LIBS     = $(PORTAUDIO) $(DSP) $(FFTS) $(PITCH) $(VEC) $(SHADER) $(TUNING) $(MEDIAN)
MAC_LIBS = $(LIBS) -framework OpenGL -framework Cocoa -framework AppKit -framework QuartzCore


WARN     = -Weverything -Wno-padded -Wno-unused-parameter -Wno-conversion
MAC_WARN = $(WARN) -Wno-direct-ivar-access -Wno-objc-missing-property-synthesis \
					-Wno-implicit-atomic-properties -Wno-auto-import


# -Ofast enables -O3, vectorization, strict aliasing, fast math
# -flto  is a link-time optimizer
# -ffast-math (?)
# -fvectorize should be enabled by default for -O3
MAC_OPTIONS = -fmodules -fobjc-arc -arch x86_64 -O3 -flto -std=c11 -mmacosx-version-min=10.7
CC = cc


all:
	make engine
	make mac_full
	make mac_trial

engine:
	$(CC) -c $(WARN) $(MAC_OPTIONS) $(LIBS) \
	src/engine/*.c -Isrc/engine
	ar rcs engine.a *.o
	rm *.o

mac_full:
	$(CC) $(MAC_WARN) $(MAC_OPTIONS) $(MAC_LIBS) \
		engine.a src/mac/*.m src/mac/*.c -Isrc/engine \
		-o full/Strobie.app/Contents/MacOS/Strobie
		ibtool --compile full/Strobie.app/Contents/Resources/Application.nib src/mac/Application.xib
		cp src/mac/Info.plist full/Strobie.app/Contents/Info.plist
		cp src/mac/Credits.rtf full/Strobie.app/Contents/Resources/Credits.rtf

mac_trial:
	$(CC) -DTRIAL $(MAC_WARN) $(MAC_OPTIONS) $(MAC_LIBS) \
		engine.a src/mac/*.m src/mac/*.c -Isrc/engine \
		-o trial/Strobie.app/Contents/MacOS/Strobie
		ibtool --compile trial/Strobie.app/Contents/Resources/Application.nib src/mac/Application.xib
		cp src/mac/Info.plist trial/Strobie.app/Contents/Info.plist
		cp src/mac/Credits.rtf trial/Strobie.app/Contents/Resources/Credits.rtf

mac_icons:
	iconutil -c icns src/app.iconset
	cp src/app.icns full/Strobie.app/Contents/Resources/app.icns
	cp src/app.icns trial/Strobie.app/Contents/Resources/app.icns
	rm src/app.icns



###########

.PHONY: test biquad
test:
	make engine
	cc $(OPTIONS) $(WARN) -Isrc/engine $(GL) $(GLFW) $(FFTS) $(PORTAUDIO) $(DSP) \
	src/test.c engine.a -o test

experiment:
	cc $(OPTIONS) $(WARN) $(FFTS) $(PORTAUDIO) src/experiment.c -o experiment

biquad:
	cc src/biquad.c $(DSP)  -o biquad
