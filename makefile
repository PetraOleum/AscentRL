CC=g++
LIBS=-lSDL2
CFLAGS=-Wall -Wextra -Werror -std=c++11 -Og
DEPS=ascentapp.h general.h region.h engine.h
OBJ=main.o ascentapp.o region.o engine.o fov.o

all: ascentrl

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< 

ascentrl: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	-rm -f *.o
	-rm -f ascentrl
