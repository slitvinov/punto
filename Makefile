DATADIR = dat
S = src
CC=c99
CFLAGS=  -Iinclude  -O2 -g
SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs`
M_LDFLAGS = -lm

O = \
$S/buffer.o\
$S/event.o\
$S/file.o\
$S/graphics.o\
$S/graphics-sdl.o\
$S/help.o\
$S/maths.o\
$S/punto.o\
$S/system.o\

punto: dirs $O
	 $(CC) $O $(LDFLAGS) $(SDL_LDFLAGS) $(M_LDFLAGS) -o bin/punto

gen:	dirs $S/gen.o
	$(CC) $S/gen.o $(M_LDFLAGS) -o bin/gen


all: dirs clean punto gen


.c.o:
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o  $@

dirs:
	mkdir -p bin
	mkdir -p data

clean:
	-rm -f $S/*.o
	-rm -fR $S/*~
	-rm -f core


.PHONY: \
all\
clean\
dirs\
gen\
punto\
