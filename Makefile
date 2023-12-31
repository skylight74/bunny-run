CC = g++
CFLAGS = -g -std=c++11 -Wall `pkg-config --cflags freetype2`
LIBS = -lglfw -lGLU -lGL -lGLEW `pkg-config --libs freetype2`
INCLUDE = -I./include

# Define your source files and target executable
SRC = $(wildcard src/*.cpp) main.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = bunny_run

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f src/*.o *.o $(EXEC)

.PHONY: all clean
