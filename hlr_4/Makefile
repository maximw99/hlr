CC=g++
CFLAGS= -c -std=c++11 -march=native -Wall -Wextra -Wpedantic -g3 -fopenmp -O3
LDFLAGS=-g3 -fopenmp

SOURCES=lodepng.cc mandelbrot.cc
HEADERS=lodepng.h complex.h

EXECUTABLE=mandelbrot

OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cc
	$(CC) $(CFLAGS) $< -o $@


clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)

distclean: clean
	$(RM) mandelbrot.png
