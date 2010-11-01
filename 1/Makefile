# Main makefile.

PROJECT = main
LDFLAGS = -lpthread -Llib/ -lstuff
MODULES = LIB CONFIG

include ~/code/c/uap/assignments/1/Makefile.common

all: $(MODULES) $(PROJECT)

main.o: main.c main.h

LIB:
	cd ./lib/ ; make

CONFIG:
	cd ./cfg/ ; make

cleanall: cleandiff cleanfifo cleanlib cleanlock cleanobj cleancfg cleanstuff

cleancfg:
	cd ./cfg/ ; make cleanall

cleanstuff:
	cd ./lib/ ; make cleanall

