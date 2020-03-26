INCLUDEDIR= ./include/
BINDIR= ./bin/
DATADIR= ./dat/
SRCPATH= ./src/

CC=gcc
CFLAGS=  -I$(INCLUDEDIR)  -O2 -g
SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs`
LDFLAGS= -L/usr/X11R6/lib -L/usr/lib -lX11 -lm -lpthread
PROFFLAGS= -pg -I$(INCLUDEDIR)	-L/usr/X11R6/lib -L/usr/lib -lX11 -lm -lpthread
DFLAGS=
AR=ar
AFLAGS=rs

PROGRAMA=$(BINDIR)punto
PROGRAMA_OBJS=$(SRCPATH)punto.o $(SRCPATH)maths.o $(SRCPATH)system.o $(SRCPATH)event.o $(SRCPATH)graphics.o $(SRCPATH)graphics-sdl.o $(SRCPATH)file.o $(SRCPATH)help.o $(SRCPATH)buffer.o
PROGRAMA_SRCS=$(SRCPATH)punto.c $(SRCPATH)maths.c $(SRCPATH)system.c $(SRCPATH)event.c $(SRCPATH)graphics.c $(SRCPATH)graphics-sdl.c $(SRCPATH)file.c $(SRCPATH)help.c $(SRCPATH)buffer.c

REDUCE_OBJS=$(SRCPATH)reducedata.o $(SRCPATH)file.o
punto: dirs $(PROGRAMA_OBJS)
	 $(CC) $(PROGRAMA_OBJS) $(LDFLAGS) $(SDL_LDFLAGS) -o $(BINDIR)punto

debug: dirs $(PROGRAMA_OBJS)
	 $(CC) $(PROGRAMA_OBJS) -I$(INCLUDEDIR) $(LDFLAGS) -g -o debug

gen:	dirs $(SRCPATH)gen.o
	$(CC) -o $(BINDIR)gen $(SRCPATH)gen.o -lm


all: dirs clean punto gen


.c.o:
	$(CC) $(CFLAGS) $(DFLAGS)  $(SDL_CFLAGS) -c $< -o  $@

dirs:
	-if [ ! -d $(BINDIR) ] ; then mkdir $(BINDIR) ; fi
	-if [ ! -d $(DATADIR) ] ; then mkdir $(DATADIR) ; fi

clean:
	-rm -f $(SRCPATH)*.o
	-rm -fR $(SRCPATH)*~
	-rm -f core

mrproper: clean
	-rm -f bin/punto
	-rm -f $(DATADIR)*.dat
	-rm -f bin/gen
	-rm -f ./debug
