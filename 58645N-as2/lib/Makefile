LIBRARY = libstuff.a
OBJECTS = arraylist.o

include ~/code/c/uap/assignments/58645N-as2/Makefile.common

all: ${LIBRARY}

${LIBRARY}: ${OBJECTS}
	${AR} ${ARFLAGS} ${LIBRARY} ${OBJECTS}

arraylist.o: arraylist.c arraylist.h

