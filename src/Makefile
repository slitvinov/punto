.POSIX:
CC = c99
CFLAGS = -O2 -g
SDL_CFLAGS = `sdl2-config --cflags`
SDL_LDFLAGS = `sdl2-config --libs`

all: punto gen
punto: punto.c; $(CC) $(CFLAGS) $(SDL_CFLAGS) punto.c -o $@ $(SDL_LDFLAGS) -lm
gen: gen.c; $(CC) $(CFLAGS) gen.c -o $@ -lm
data: gen; mkdir -p data && ./gen
clean:; -rm -f punto gen
