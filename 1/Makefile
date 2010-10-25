# Main makefile.

#PROJECT = main
MODULES = CONFIG STUFF

include ~/code/c/uap/assignments/1/Makefile.common

all: $(MODULES) $(PROJECT)

CONFIG:
	cd ./cfg/ ; make

STUFF:
	cd ./stuff/ ; make

clean: cleanlib cleanobj cleancfg cleanstuff

cleancfg:
	cd ./cfg/ ; make cleanall

cleanstuff:
	cd ./stuff/ ; make cleanall

