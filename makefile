CXX=g++
CFLAGS= -std=c++17 -g -DDEBUG

all:: judger

judger: judge_core.cpp
	$(CXX) $(CFLAGS) -o $@ $<

