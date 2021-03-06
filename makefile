CC=g++
LIBS=-lSDL2 -lSDL2_ttf
CFLAGS=-Wall -Wextra -Werror -std=c++11 -Og
DEPS=ascentapp.h general.h region.h engine.h creature.h inventory.h
OBJ=main.o ascentapp.o region.o engine.o fov.o creature.o inventory.o

all: ascentrl

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< 

ascentrl: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	-rm -f *.o
	-rm -f ascentrl
