# ©2014 Cameron Desrochers

ifeq ($(OS),Windows_NT)
	EXT=.exe
	PLATFORM_OPTS=-static
else
	EXT=
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		PLATFORM_OPTS=
	else
		PLATFORM_OPTS=-Wl,--no-as-needed -lrt
	endif
endif

default: benchmarks$(EXT)

benchmarks$(EXT): bench.cpp ../readerwriterqueue.h ../readerwritercircularbuffer.h ../atomicops.h ext/1024cores/spscqueue.h ext/folly/ProducerConsumerQueue.h ../tests/common/simplethread.h ../tests/common/simplethread.cpp systemtime.h systemtime.cpp makefile
	g++ -std=c++11 -Wpedantic -Wall -DNDEBUG -O3 -g bench.cpp ../tests/common/simplethread.cpp systemtime.cpp -o benchmarks$(EXT) -pthread $(PLATFORM_OPTS)

run: benchmarks$(EXT)
	./benchmarks$(EXT)
