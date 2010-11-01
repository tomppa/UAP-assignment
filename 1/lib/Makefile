LIBRARY = libstuff.a
OBJECTS = fops.o pops.o

include ~/code/c/uap/assignments/1/Makefile.common

all: ${LIBRARY}

${LIBRARY}: ${OBJECTS}
	${AR} ${ARFLAGS} ${LIBRARY} ${OBJECTS}

fops.o: fops.c fops.h
pops.o: pops.c pops.h

