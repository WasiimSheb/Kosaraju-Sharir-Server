# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -pthread

# Target executable
TARGET = kosaraju_server

# Source files
SRCS = kosaraju_server.cpp proactor.cpp

# Header files
HDRS = kosaraju_server.hpp proactor.hpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean