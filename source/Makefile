CFLAGS = `sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -ansi -Wfloat-equal
INCS = teletext.h
TARGET = teletext
SOURCES =  teletext.c main.c
LIBS =  `sdl2-config --libs` -lm
CC = gcc


all: $(TARGET)

$(TARGET): $(SOURCES) $(INCS)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET) 

memchk: teletext
	valgrind --error-exitcode=1 --quiet --leak-check=full ./teletext ../test.m7

.PHONY: clean run memchk all
