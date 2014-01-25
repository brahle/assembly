default: debug

SOURCES=src/main.cpp src/layout/union_find.cpp
HEADERS=src/layout/union_find.hpp

include buildnumber.mak

OPTIMIZATION_FLAGS=-O3 -flto -finline-limit=200
ASORTED_FLAGS=-std=c++11
WARINIG_FLAGS=-Wall -Wextra -Wno-unused-result
FLAGS=$(OPTIMIZATION_FLAGS) $(ASORTED_FLAGS) $(WARNING_FLAGS) $(BUILD_NUMBER_LDFLAGS)
DEBUG_FLAGS=-g -ggdb -pg
NODEBUG_FLAGS=-s

release: $(SOURCES) $(HEADERS) $(BUILD_NUMBER_FILE)
	g++ $(FLAGS) $(NODEBUG_FLAGS) $(SOURCES) -o bin/$@

debug: $(SOURCES) $(HEADERS) $(BUILD_NUMBER_FILE)
	g++ $(FLAGS) $(DEBUG_FLAGS) $(SOURCES) -o bin/$@

valgrind: debug
	valgrind bin/debug

clean:
	rm bin/*

