CC=g++
CFLAGS=-c -g -Wall
LDFLAGS=
SOURCES=src/main.cpp src/dataManager.cpp src/pagesManager.cpp src/FileManager.cpp src/System.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/db

all: $(SOURCES) $(EXECUTABLE) clean
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) *.o
