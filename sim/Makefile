CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = sim
SRCS = sim_loop.cpp scheduler.cpp instructions.cpp memory.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	del $(TARGET).exe
