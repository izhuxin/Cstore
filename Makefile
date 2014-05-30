CC=g++
CFLAGS=-c -g -Wall
LDFLAGS=
SOURCES=src/main.cpp src/Page.cpp src/pagesManager.cpp src/OperandBrain.cpp src/SortBrain.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/db

all: $(SOURCES) $(EXECUTABLE) clean
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) *.o
