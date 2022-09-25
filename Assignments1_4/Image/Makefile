TARGET = Image
SOURCE = bmp.cpp image.cpp image.todo.cpp jpeg.cpp lineSegments.cpp lineSegments.todo.cpp



TARGET_LIB = lib$(TARGET).a

CFLAGS += -I. -I.. -std=c++14 -Wunused-result

CFLAGS_DEBUG = -DDEBUG -g3
CFLAGS_RELEASE = -O3 -DRELEASE -funroll-loops -ffast-math -DNDEBUG

SRC = ./
BIN = ../
BIN_O = ../Bin/Linux/Release/$(TARGET)/
INCLUDE = /usr/include/

CC  = gcc
CXX = g++
MD  = mkdir
AR  = ar

OBJECTS=$(addprefix $(BIN_O), $(addsuffix .o, $(basename $(SOURCE))))

all: CFLAGS += $(CFLAGS_RELEASE)
all: $(BIN)
all: $(BIN_O)
all: $(BIN)$(TARGET_LIB)

allNoGL: CFLAGS += $(CFLAGS_RELEASE)
allNoGL: $(BIN)
allNoGL: $(BIN_O)
allNoGL: $(BIN)$(TARGET_LIB)

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(BIN)
debug: $(BIN)$(TARGET_LIB)

clean:
	rm -f $(BIN)$(TARGET_LIB)
	rm -f $(OBJECTS)

$(BIN):
	$(MD) -p $(BIN)

$(BIN_O):
	$(MD) -p $(BIN_O)

$(BIN)$(TARGET_LIB): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(BIN_O)%.o: $(SRC)%.c
	$(CC) -c -o $@ $(CFLAGS) -I$(INCLUDE) $<

$(BIN_O)%.o: $(SRC)%.cpp
	$(CXX) -c -o $@ $(CFLAGS) -I$(INCLUDE) $<
