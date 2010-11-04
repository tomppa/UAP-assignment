# Main makefile.

PROJECT = main
MODULES = MODULE_X LIBRARY

include ~/code/c/uap/assignments/58645N-as2/Makefile.common

all: $(PROJECT)

main.o: main.c main.h

MODULE_X:
	cd ./mod_x/ ; make

LIBRARY:
	cd ./lib/ ; make

cleanall: cleanfifo cleanlib cleanlock cleanobj cleanmodx cleanlibrary

cleanmodx:
	cd ./mod_x/ ; make cleanall

cleanlibrary:
	cd ./lib/ ; make cleanall

