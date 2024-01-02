

ifeq ($(OS),Windows_NT)
	EXT=.exe
	PLATFORM_OPTS=-static
	PLATFORM_LD_OPTS=-Wl,--no-as-needed
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		EXT=
		PLATFORM_OPTS=
		PLATFORM_LD_OPTS=
	else
		EXT=
		PLATFORM_OPTS=
		PLATFORM_LD_OPTS=-lrt -Wl,--no-as-needed
	endif
endif


default: unittests$(EXT)

unittests$(EXT): unittests.cpp ../../readerwriterqueue.h ../../readerwritercircularbuffer.h ../../atomicops.h ../common/simplethread.h ../common/simplethread.cpp minitest.h makefile
	g++ $(PLATFORM_OPTS) -std=c++11 -Wsign-conversion -Wpedantic -Wall -DNDEBUG -O3 -g unittests.cpp ../common/simplethread.cpp -o unittests$(EXT) -pthread $(PLATFORM_LD_OPTS)

run: unittests$(EXT)
	./unittests$(EXT)
