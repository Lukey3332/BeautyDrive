CC=gcc
CFLAGS=-I. `pkg-config --cflags gstreamer-1.0` `sdl-config --cflags`

LIBS=`pkg-config --libs gstreamer-1.0` `sdl-config --libs`

%.o: %.c *.h
	$(CC) -c -o $@ $< $(CFLAGS)

BeautyDrive: sys_sdl.o vid_sdl.o dec_gst.o in_sdl.o game.o key.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o core
