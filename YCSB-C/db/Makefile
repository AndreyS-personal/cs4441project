CC=g++
CFLAGS=-std=c++17 -c -g -Wall -fgnu-tm

INCLUDES=-I../ \
         -I../bw-tree/include \
         -I../bw-tree/external \
         -I../bw-tree/external/memory-manager/include \
		 -I../bw-tree/external/memory-manager/external/cpp-utility/include
# INCLUDES=-I../ -I../bw-tree/include
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) $(OBJECTS)

