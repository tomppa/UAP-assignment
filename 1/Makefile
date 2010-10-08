CC =		gcc
CFLAGS = 	-O2 -g -Wall -pedantic -pg -std=c99
LDFLAGS =	-lstuff -L.
CFLAGS +=	-D_GNU_SOURCE

SUFFIXES = .c .o
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
LIBS = $(wildcard lib*.a)
DIFFS = $(wildcard *.diff)
PROJECT = main

.PHONY: all cleanall cleanobj cleandiff cleanlib

all: $(PROJECT)

.c.o:
	$(CC) $(CFLAGS) -c $<

$PROJECT: $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@

libstuff.a: config.o stuff.o
	ar rvs libstuff.a $?

main.o: main.c stuff.o config.o
config.o: config.c config.h
stuff.o: stuff.c stuff.h

cleanall: cleanobj cleandiff cleanlib
	rm -f $(PROGS)

cleanobj:
	rm -f $(OBJECTS)

cleandiff:
	rm -f $(DIFFS)

cleanlib:
	rm -f $(LIBS)

