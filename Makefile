#
# Copyright (C) 2013 Davorin Šego
#

JSON      = --pkg json-glib-1.0 -X -I/usr/local/include/json-glib-1.0 -X -ljson-glib-1.0

SDL       = --pkg sdl -X -I/usr/include/SDL

CAIRO     = --pkg cairo -X -I/usr/local/include/cairo -X -lcairo

PANGO     = --pkg pango --pkg pangocairo -X -I/usr/local/include/pango -X -lpango-1.0 -X -lpangocairo-1.0

CAIRO_SDL = ../../vala-extra-vapis/cairosdl.vapi -X -I../cairosdl ../cairosdl/cairosdl.c

PORTAUDIO = ../../vala-extra-vapis/portaudio.vapi ../../vala-extra-vapis/pa_ringbuffer.vapi \
            ../portaudio/src/common/pa_ringbuffer.c -X -I../portaudio/include \
            -X -I../portaudio/src/common -X -lportaudio

KISS_FFT  = ../../vala-extra-vapis/kiss_fft.vapi -X -I../kiss_fft130 \
            ../kiss_fft130/kiss_fft.c ../kiss_fft130/tools/kiss_fftr.c

SNDFILE   = ../../vala-extra-vapis/sndfile.vapi -X -I../libsndfile-1.0.25/src -X -lsndfile




all:
	valac -g -o \
        strobie app.vala converter.vala strobe.vala sdl_cairo_window.vala biquad.vala \
	      pitch_estimation.vala tuning.vala config.vala display.vala \
	      ../src/src.vala ../fir/window.vala ../fir/simple_filter.vala \
	      --pkg posix --pkg gee-0.8 \
	      $(PORTAUDIO) $(KISS_FFT) $(CAIRO) $(PANGO) $(SDL) $(CAIRO_SDL) $(JSON)

test_lowpass:
	valac -o test_lowpass test_lowpass.vala biquad.vala \
	         --pkg posix \
	         $(SNDFILE)