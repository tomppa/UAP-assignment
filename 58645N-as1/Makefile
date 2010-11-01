# Main makefile.

PROJECT = main
MODULES = LIB CONFIG

include ~/code/c/uap/assignments/1/Makefile.common

all: $(PROJECT)

main.o: main.c main.h

LIB:
	cd ./lib/ ; make

CONFIG:
	cd ./cfg/ ; make

cleanall: cleanfifo cleanlib cleanlock cleanobj cleancfg cleanstuff

cleancfg:
	cd ./cfg/ ; make cleanall

cleanstuff:
	cd ./lib/ ; make cleanall

