CFLAGS = `sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -ansi -Wfloat-equal
INCS = ../source/teletext.h
TARGET = test
SOURCES =  ../source/teletext.c test.c
LIBS =  `sdl2-config --libs` -lm
CC = gcc


all: $(TARGET)

$(TARGET): $(SOURCES) $(INCS)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET) 

memchk: test
	valgrind --error-exitcode=1 --quiet --leak-check=full ./test

.PHONY: clean run memchk all
