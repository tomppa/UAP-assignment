CC =		gcc
CFLAGS = 	-O2 -g -Wall -pedantic -pg -std=c99
LDFLAGS =	-lstuff -L.
CFLAGS +=	-D_GNU_SOURCE

SUFFIXES = .c .o
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
DIFFS = $(wildcard *.diff)
PROJECT = main

.PHONY: all cleanall cleanobj cleandiff

all: $(PROJECT)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROJECT): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

main.o: main.c stuff.o config.o
config.o: config.c config.h
stuff.o: stuff.c stuff.h

cleanall: cleanobj cleandiff
	rm -f $(PROGS)

cleanobj:
	rm -f $(OBJECTS)

cleandiff:
	rm -f $(DIFFS)

