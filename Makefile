DATADIR = dat
S = src
CC = c99
CFLAGS = -O2 -g
SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs`
M_LDFLAGS = -lm
M_CFLAGS = -Iinclude

O = \
$S/buffer.o\
$S/event.o\
$S/file.o\
$S/graphics.o\
$S/graphics-sdl.o\
$S/help.o\
$S/maths.o\
$S/punto.o\

all: dirs clean punto gen
punto: dirs $O
	 $(CC) $O $(LDFLAGS) $(SDL_LDFLAGS) $(M_LDFLAGS) -o bin/punto

gen:	dirs $S/gen.o
	$(CC) $S/gen.o $(M_LDFLAGS) -o bin/gen



.c.o:
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(M_CFLAGS) -c $< -o  $@

dirs:
	mkdir -p bin dat

clean:
	-rm -f $O bin/punto bin/gen


.PHONY: \
all\
clean\
dirs\
gen\
punto\
