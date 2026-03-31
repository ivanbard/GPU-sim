CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = sim
SRCS = sim_loop.cpp scheduler.cpp instructions.cpp memory.cpp

.PHONY: all clean test

all: $(TARGET)

test: $(TARGET)
	./test.sh

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
