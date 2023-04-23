.POSIX:
.SUFFIXES:
.SUFFIXES: .o
.SUFFIXES: .c
CC = c99
CFLAGS = -O2 -g
SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs`

O = \
buffer.o\
event.o\
file.o\
graphics.o\
graphics-sdl.o\
maths.o\

all: punto gen
.c.o:; $(CC) -c $(CFLAGS) $(SDL_CFLAGS) $<
.c:; $(CC) $(LDFLAGS) $(SDL_CFLAGS) $< $O -o $@ $(SDL_LDFLAGS) -lm
punto: $O
gen: $O
data: gen
	mkdir data && ./gen
clean:; -rm -f $O punto gen dat/* && rmdir dat
dep:; ../script/dep *.c | sed -e 's,punto\.o,punto,g' -e 's,gen\.o,gen,g' > dep.mk
include dep.mk
