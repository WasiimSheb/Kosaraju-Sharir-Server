# Variables
CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = kosaraju_server
SRC = kosaraju_server.cpp
HEADER = kosaraju_server.hpp
OBJ = kosaraju_server.o

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Rule to compile the source file
$(OBJ): $(SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) -c $(SRC)

# Clean up
clean:
	rm -f $(TARGET) $(OBJ)

# Phony targets
.PHONY: all clean
