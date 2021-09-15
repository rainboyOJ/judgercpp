CXX=g++
CFLAGS= -std=c++17

judger: src/judge_core.cpp
	$(CXX) $(CFLAGS) -o $@ $<

