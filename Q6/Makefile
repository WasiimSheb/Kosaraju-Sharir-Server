CXX = g++
CXXFLAGS = -std=c++11 -Wall

TARGET = kosaraju_reactor
OBJS = kosaraju_reactor.o reactor.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

kosaraju_server.o: kosaraju_server.cpp  reactor.hpp
	$(CXX) $(CXXFLAGS) -c kosaraju_reactor.cpp

reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(CXXFLAGS) -c reactor.cpp

clean:
	rm -f $(TARGET) $(OBJS)
