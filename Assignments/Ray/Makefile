TARGET = Ray
SOURCE = GLSLProgram.cpp mouse.cpp mouse.cpp camera.cpp cone.todo.cpp directionalLight.cpp shapeList.cpp pointLight.cpp scene.todo.cpp spotLight.cpp triangle.todo.cpp box.cpp camera.todo.cpp cylinder.cpp directionalLight.todo.cpp shapeList.todo.cpp pointLight.todo.cpp sphereLight.cpp sphereLight.todo.cpp sphere.cpp spotLight.todo.cpp window.cpp box.todo.cpp cone.cpp cylinder.todo.cpp fileInstance.cpp scene.cpp sphere.todo.cpp triangle.cpp shape.cpp torus.cpp torus.todo.cpp

TARGET_LIB = lib$(TARGET).a

CFLAGS += -I. -I.. -Wunused-result -g
CPPFLAGS += -I. -I.. -std=c++14 -Wunused-result

CPPFLAGS_DEBUG = -DDEBUG -g3 -g
CPPFLAGS_RELEASE = -O3 -DRELEASE -funroll-loops -ffast-math -DNDEBUG

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
all: CPPFLAGS += $(CFLAGS_RELEASE)
all: $(BIN)
all: $(BIN_O)
all: $(BIN)$(TARGET_LIB)

debug: CFLAGS += $(CPPFLAGS_DEBUG)
debug: CPPFLAGS += $(CPPFLAGS_DEBUG)
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
	$(CXX) -c -o $@ $(CPPFLAGS) -I$(INCLUDE) $<
