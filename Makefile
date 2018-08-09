CFLAGS=-Wall -Wextra
LDFLAGS=-lSDL2 -lSDL2_ttf -lSDL2_image -lm

BIN=tsp

${BIN}: main.c tsp.o solver.o

.PHONY: all clean

all: ${BIN}

clean:
	-rm -f ${BIN} *.o
