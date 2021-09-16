CXX=g++
CXXFLAGS= -std=c++17 -O2
debug=no
#本地使用，不需要sudo权限，不加载任何seccomp rule
local=yes
ifeq (yes, ${debug})
CXXFLAGS += -DDEBUG
#$(info ************ DEBUG VERSIOIN ************)
#else
#$(info ************ RELEASE VERSIOIN **********)
endif

ifeq (yes, ${local})
CXXFLAGS += -DLOCAL
endif

all:: judger judger_core

judger_core: judge_core.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

judger: judge_wrapper.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean::
	rm judger_core judger
