CC=gcc
SDL_VERSION=2
GSTREAMER_VERSION=1.0

ifneq (,$(filter $(SDL_VERSION),1 1.2))
SDL_LIBS=sdl
SDL_OBJS=sys_sdl.o vid_sdl.o in_sdl.o
else
SDL_LIBS=sdl2
SDL_OBJS=sys_sdl2.o vid_sdl2.o in_sdl.o
endif

ifneq (,$(filter $(GSTREAMER_VERSION),0.1 0.10))
GSTREAMER_LIBS=gstreamer-0.10 gstreamer-app-0.10
GSTREAMER_OBJS=dec_gst0.10.o
else
GSTREAMER_LIBS=gstreamer-1.0 gstreamer-app-1.0
GSTREAMER_OBJS=dec_gst.o
endif

CFLAGS=-I. -g -O2 -Wall -pedantic `pkg-config --cflags $(SDL_LIBS) $(GSTREAMER_LIBS)`
LIBS=-lm -lGLU -lpng `pkg-config --libs $(SDL_LIBS) $(GSTREAMER_LIBS)`

%.o: %.c *.h
	$(CC) -c -o $@ $< $(CFLAGS)

BeautyDrive: $(SDL_OBJS) $(GSTREAMER_OBJS) physics_minimal.o renderer.o game.o key.o map.o vector.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o core
