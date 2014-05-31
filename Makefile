CC=g++
CFLAGS=-c -g -Wall
SOURCE_DIR= ./src
SOURCES=src/main.cpp \
        src/OperandBrain.cpp \
        src/pagesManager.cpp \
        src/Page.cpp \
        src/SortBrain.cpp

OBJECTS_DIR=./bin
OBJECTS=$(OBJECTS_DIR)/main.o \
        $(OBJECTS_DIR)/OperandBrain.o \
        $(OBJECTS_DIR)/Page.o \
        $(OBJECTS_DIR)/pagesManager.o \
        $(OBJECTS_DIR)/SortBrain.o

EXECUTABLE=bin/db

all: $(OBJECTS_DIR_CREATE) $(EXECUTABLE)

$(OBJECTS_DIR_CREATE):
	mkdir bin
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

$(OBJECTS_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ $<

