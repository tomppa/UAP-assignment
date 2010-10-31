# Main makefile.

PROJECT = main
MODULES = CONFIG STUFF

include ~/code/c/uap/assignments/1/Makefile.common

all: $(MODULES) $(PROJECT)

main.o: main.c main.h

CONFIG:
	cd ./cfg/ ; make

STUFF:
	cd ./stuff/ ; make

cleanall: cleandiff cleanfifo cleanlib cleanlock cleanobj cleancfg cleanstuff

cleancfg:
	cd ./cfg/ ; make cleanall

cleanstuff:
	cd ./stuff/ ; make cleanall

