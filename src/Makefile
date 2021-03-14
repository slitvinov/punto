.POSIX:
.SUFFIXES:
.SUFFIXES: .o
.SUFFIXES: .c
CC = c99
CFLAGS = -O2 -g
SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs`
M_CFLAGS = -Iinclude

O = \
buffer.o\
event.o\
file.o\
graphics.o\
graphics-sdl.o\
help.o\
maths.o\

all: punto gen
.c.o:; $(CC) -c $(CFLAGS) $(SDL_CFLAGS) $(M_CFLAGS) $< -o $@ 
.c:; $(CC) $(LDFLAGS) $(SDL_CFLAGS) $< $O -o $@ $(SDL_LDFLAGS) -lm
punto: $O
gen: $O
data: gen
	mkdir dat && ./gen
clean:; -rm -f $O punto gen dat/* && rmdir dat
