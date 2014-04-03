# Copyright (C) Davorin Šego

FFTS = -I/usr/local/include/ffts /usr/local/lib/libffts.a

# to build PortAudio: CFLAGS="-w -arch x86_64" ./configure  --enable-mac-universal=NO && make
PORTAUDIO = ../portaudio/trunk/src/common/pa_ringbuffer.c \
					-I../portaudio/trunk/include \
					-I../portaudio/trunk/src/common \
						../portaudio/trunk/lib/.libs/libportaudio.dylib

DSP      = ../biquad/src/biquad.c -I../biquad/src	../interpolator/src/intp.c -I../interpolator/src
VEC      = ../buffer/src/buffer.c -I../buffer/src
PITCH    = ../pitch/src/nsdf.c -I../pitch/src
TUNING   = ../tuning/src/equal-temp.c -I../tuning/src
SHADER   = ../shader/src/shader.c -I../shader/src
MEDIAN   = ../median/src/median.c -I../median/src
GLFW     = -lglfw3
LIBS     = $(PORTAUDIO) $(DSP) $(FFTS) $(PITCH) $(VEC) $(SHADER) $(TUNING) $(MEDIAN)
MAC_LIBS = $(LIBS) -framework OpenGL -framework Cocoa -framework AppKit -framework QuartzCore \
		../read-url/read-url.c -I ../read-url -lcurl \
		../NSWindowButtons/NSWindowButtons/NSWindow+AccessoryView.m -I../NSWindowButtons/NSWindowButtons


WARN     = -Weverything -Wno-padded -Wno-unused-parameter -Wno-conversion -Wno-disabled-macro-expansion
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
	make mac

engine:
	$(CC) -c $(WARN) $(MAC_OPTIONS) $(LIBS) \
	src/engine/*.c -Isrc/engine
	ar rcs engine.a *.o
	rm *.o

mac:
	rm -rfd Strobie.app
	mkdir -p Strobie.app/Contents/Resources
	mkdir -p Strobie.app/Contents/MacOS
	cp src/mac/resources/* Strobie.app/Contents/Resources
	$(CC) $(MAC_WARN) $(MAC_OPTIONS) $(MAC_LIBS) \
		engine.a src/mac/*.m src/mac/*.c -Isrc/engine \
		-o Strobie.app/Contents/MacOS/Strobie
		ibtool --compile Strobie.app/Contents/Resources/Application.nib src/mac/Application.xib
		cp src/mac/Info.plist Strobie.app/Contents/Info.plist

mac-icons:
	iconutil -c icns src/app.iconset --output src/mac/resources/app.icns
	# rm src/app.icns


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
