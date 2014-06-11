# Copyright (C) Davorin Šego

FFTS := -I/usr/local/include/ffts /usr/local/lib/libffts.a

# to build PortAudio: CFLAGS="-w -arch x86_64" ./configure  --enable-mac-universal=NO && make
PORTAUDIO := ../portaudio/trunk/src/common/pa_ringbuffer.c \
							-I../portaudio/trunk/include \
							-I../portaudio/trunk/src/common \
							../portaudio/trunk/lib/.libs/libportaudio.a
DSP			:= ../biquad/src/biquad.c -I../biquad/src	../interpolator/src/intp.c -I../interpolator/src
VEC			:= ../buffer/src/buffer.c -I../buffer/src
PITCH		:= ../pitch/src/nsdf.c -I../pitch/src
TUNING	:= ../tuning/src/equal-temp.c -I../tuning/src
SHADER	:= ../shader/src/shader.c -I../shader/src
MEDIAN	:= ../median/src/median.c -I../median/src
GLFW		:= -lglfw3
LIBS 		:= $(PORTAUDIO) $(DSP) $(FFTS) $(PITCH) $(VEC) $(SHADER) $(TUNING) $(MEDIAN)


MAC_LIBS := -framework AudioToolbox -framework CoreAudio -framework AudioUnit \
						-framework OpenGL -framework Cocoa -framework AppKit -framework QuartzCore


WARN := -Weverything -Wno-padded -Wno-unused-parameter -Wno-conversion
# -fsanitize=undefined-trap -fsanitize-undefined-trap-on-error

# -Wno-disabled-macro-expansion -Wno-deprecated-declarations


MAC_WARN := $(WARN) -Wno-direct-ivar-access -Wno-objc-missing-property-synthesis \
						-Wno-implicit-atomic-properties -Wno-auto-import


# -Ofast enables -O3, vectorization, strict aliasing, fast math
# -flto  is a link-time optimizer
# -ffast-math (?)
# -fvectorize should be enabled by default for -O3
MAC_OPTIONS := -fmodules -fobjc-arc -arch x86_64 -O3 -flto -std=c11 -mmacosx-version-min=10.7
CC := cc


mac_full:
	$(MAKE) mac_bundle BUNDLE="StrobieFull.app"

mac_trial:
	$(MAKE) mac_bundle BUNDLE="StrobieTrial.app" TRIAL_VERSION="-DTRIAL_VERSION"

mac_bundle: engine.a
	rm -rfd $(BUNDLE)
	mkdir -p $(BUNDLE)/Contents/Resources
	mkdir -p $(BUNDLE)/Contents/MacOS
	cp src/mac/resources/* $(BUNDLE)/Contents/Resources
	cp src/mac/Info.plist $(BUNDLE)/Contents/Info.plist
	cp src/mac/Strobie.entitlements $(BUNDLE)/Contents/Strobie.entitlements
	$(CC) $(MAC_WARN) $(MAC_OPTIONS) $(MAC_LIBS) $(LIBS) $(TRIAL_VERSION) \
		engine.a src/mac/*.m src/mac/*.c -Isrc/engine -o $(BUNDLE)/Contents/MacOS/Strobie
	ibtool --compile $(BUNDLE)/Contents/Resources/Application.nib src/mac/Application.xib

mac_icons: src/app.iconset
	iconutil -c icns src/app.iconset --output src/mac/resources/app.icns

engine.a: src/engine/*.c
	$(CC) -c $(WARN) $(MAC_OPTIONS) $(LIBS) src/engine/*.c -Isrc/engine
	ar rcs engine.a *.o
	rm *.o

mac_pkg:
	cp -r StrobieFull.app/ Strobie.app/
	codesign \
		--force --deep --sign  "3rd Party Mac Developer Application: Davorin Sego" \
		--entitlements src/mac/Strobie.entitlements \
		Strobie.app
	productbuild \
		--component Strobie.app /Applications \
		--sign "3rd Party Mac Developer Installer: Davorin Sego" \
		--product src/mac/Info.plist Strobie.pkg
	rm -rfd Strobie.app


###########

# .PHONY: test biquad
# test: engine
# 	cc $(OPTIONS) $(WARN) -Isrc/engine $(GL) $(GLFW) $(FFTS) $(PORTAUDIO) $(DSP) \
# 	src/test.c engine.a -o test

# experiment:
# 	cc $(OPTIONS) $(WARN) $(FFTS) $(PORTAUDIO) src/experiment.c -o experiment

# biquad:
# 	cc src/biquad.c $(DSP)  -o biquad
