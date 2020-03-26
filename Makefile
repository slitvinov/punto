####
# Makefile para compilar punto.c
#
####

#directorios de la aplicacion
INCLUDEDIR= ./include/
BINDIR= ./bin/
DATADIR= ./dat/
SRCPATH= ./src/
INCSDL= /usr/include/SDL 

#gcc  -O3 -funroll-all-loops -fomit-frame-pointer -ffast-math -march=i686 

#programas estandar que se van a utilizar y opciones de los mismos:
CC=gcc
CFLAGS=  -Wall -I$(INCLUDEDIR)  -march=i686  -O2 -ffast-math -D_REENTRANT -g
LDFLAGS= -L/usr/X11R6/lib -L/usr/lib -lX11 -lm -lSDL -lpthread
PROFFLAGS= -pg -I$(INCLUDEDIR) 	-L/usr/X11R6/lib -L/usr/lib -lX11 -lm -lSDL -lpthread
DFLAGS= 
AR=ar
AFLAGS=rs

#programa ejecutable
PROGRAMA=$(BINDIR)punto
PROGRAMA_OBJS=$(SRCPATH)punto.o $(SRCPATH)maths.o $(SRCPATH)system.o $(SRCPATH)event.o $(SRCPATH)graphics.o $(SRCPATH)graphics-sdl.o $(SRCPATH)file.o $(SRCPATH)help.o $(SRCPATH)buffer.o
PROGRAMA_OBJSprof=punto.o maths.o system.o event.o graphics.o file.o help.o
PROGRAMA_SRCS=$(SRCPATH)punto.c $(SRCPATH)maths.c $(SRCPATH)system.c $(SRCPATH)event.c $(SRCPATH)graphics.c $(SRCPATH)graphics-sdl.c $(SRCPATH)file.c $(SRCPATH)help.c $(SRCPATH)buffer.c 

REDUCE_OBJS=$(SRCPATH)reducedata.o $(SRCPATH)file.o

#generacion del programa

punto: dirs $(PROGRAMA_OBJS)
	 $(CC) $(PROGRAMA_OBJS) $(CFLAGS) $(LDFLAGS)  -o $(BINDIR)punto

prof: dirs 
	 $(CC)	 $(PROGRAMA_SRCS) $(PROFFLAGS) -I$(INCSDL) -lm -c 
	 $(CC) $(PROGRAMA_OBJSprof) $(PROFFLAGS) $(CFLAGS) $(LDFLAGS)  -o $(BINDIR)pfpunto

reduce: dirs $(REDUCE_OBJS)
	 $(CC) $(REDUCE_OBJS) $(CFLAGS) $(LDFLAGS)  -o $(BINDIR)reduce

debug: dirs $(PROGRAMA_OBJS) 
	 $(CC) $(PROGRAMA_OBJS) -I$(INCLUDEDIR) $(LDFLAGS) -g -o debug 

gen: 	dirs $(SRCPATH)gen.o
	$(CC) -o $(BINDIR)gen $(SRCPATH)gen.o -lm	


all: dirs clean punto gen


.c.o:	
	$(CC) $(CFLAGS) $(DFLAGS)  -I$(INCSDL) -c $< -o  $@ 

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



