# Makefile for CPU Scheduler Simulator

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
BIN = sim
SRCS = main.cpp process.cpp event.cpp scheduler.cpp simulator.cpp statistics.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS)