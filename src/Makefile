.POSIX:
.SUFFIXES:
.SUFFIXES: .o
.SUFFIXES: .c
CC = c99
CFLAGS = -O2 -g
SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs`
M_LDFLAGS = -lm
M_CFLAGS = -Iinclude

O = \
buffer.o\
event.o\
file.o\
graphics.o\
graphics-sdl.o\
help.o\
maths.o\
punto.o\

all: dirs clean punto gen
.c.o:; $(CC) $(CFLAGS) $(SDL_CFLAGS) $(M_CFLAGS) -c $< -o  $@
.c:; $(CC) $O $(LDFLAGS) $(SDL_LDFLAGS) $(M_LDFLAGS) -o punto
punto: $O
gen: $O

clean:; -rm -f $O punto gen
