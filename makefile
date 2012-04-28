# Makefile

CC=gcc
CFLAGS=-lm -g -lX11 -Wall -pedantic --std=c99

all: box_stack

box_stack: main.o gui_x11.o box.o genetic.o const.h genetic.h gui.h box.h
	$(CC) $(CFLAGS) main.o gui_x11.o box.o genetic.o -o box_stack

main.o: main.c box.h genetic.h gui.h
	$(CC) $(CFLAGS) -c main.c

gui_x11.o: gui_x11.c gui.h box.h genetic.h
	$(CC) $(CFLAGS) -c gui_x11.c

box.o: box.c box.h
	$(CC) $(CFLAGS) -c box.c

genetic.o: genetic.c genetic.h box.h
	$(CC) $(CFLAGS) -c genetic.c


clean: 
	rm -f *.mod *.o box_stack
