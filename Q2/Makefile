# Makefile

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -pg

# Targets
TARGET_VECTOR_VEC = kosarajuVectorVec
TARGET_VECTOR_LIST = kosarajuVectorList
TARGET_LIST = kosarajuList
TARGET_DEQUE = kosarajuDeque

# Source files
SRC_VECTOR_VEC = kosarajuVectorVec.cpp
SRC_VECTOR_LIST = kosarajuVectorList.cpp
SRC_LIST = kosarajuList.cpp
SRC_DEQUE = kosarajuDeque.cpp

# Header file
HEADER = kosaraju_scc.hpp

# Rules
all: $(TARGET_VECTOR_VEC) $(TARGET_VECTOR_LIST) $(TARGET_LIST) $(TARGET_DEQUE)

$(TARGET_VECTOR_VEC): $(SRC_VECTOR_VEC) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $(TARGET_VECTOR_VEC) $(SRC_VECTOR_VEC)

$(TARGET_VECTOR_LIST): $(SRC_VECTOR_LIST) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $(TARGET_VECTOR_LIST) $(SRC_VECTOR_LIST)

$(TARGET_LIST): $(SRC_LIST) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $(TARGET_LIST) $(SRC_LIST)

$(TARGET_DEQUE): $(SRC_DEQUE) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $(TARGET_DEQUE) $(SRC_DEQUE)

generate_graph:
	python3 randomGraph.py

clean:
	rm -f $(TARGET_VECTOR_VEC) $(TARGET_VECTOR_LIST) $(TARGET_LIST) $(TARGET_DEQUE) 

run_vector_vec: $(TARGET_VECTOR_VEC) generate_graph
	./$(TARGET_VECTOR_VEC)

run_vector_list: $(TARGET_VECTOR_LIST) generate_graph
	./$(TARGET_VECTOR_LIST)

run_list: $(TARGET_LIST) generate_graph
	./$(TARGET_LIST)

run_deque: $(TARGET_DEQUE) generate_graph
	./$(TARGET_DEQUE)

profile_vector_vec: run_vector_vec
	gprof $(TARGET_VECTOR_VEC) gmon.out > analysis_vector_vec.txt

profile_vector_list: run_vector_list
	gprof $(TARGET_VECTOR_LIST) gmon.out > analysis_vector_list.txt

profile_list: run_list
	gprof $(TARGET_LIST) gmon.out > analysis_list.txt

profile_deque: run_deque
	gprof $(TARGET_DEQUE) gmon.out > analysis_deque.txt

.PHONY: all clean run_vector_vec run_vector_list run_list run_deque profile_vector_vec profile_vector_list profile_list profile_deque generate_graph
