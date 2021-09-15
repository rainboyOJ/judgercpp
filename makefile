CXX=g++
CFLAGS= -std=c++17 -g -DDEBUG

judger: src/judge_core.cpp
	$(CXX) $(CFLAGS) -o $@ $<

