default: run

INCLUDE=./src
VPATH=src:src/layout:src/overlap:bin

# these headers are different. first, they don't exists
# but *.hpp files exists. it means that they contain template
# implementation and everything must be recompiled when
# they are changed.
HPP=util.h

OBJ=fm_index.o overlap.o better_overlap.o better_read.o read.o sort.o suffix_array.o suffix_filter.o unitigging.o util.o
OBJ_SPECIAL=sais.o
ALL_OBJ=$(OBJ) $(OBJ_SPECIAL)

TEST=

EXE=main_layout

include buildnumber.mak

.PHONY: run valgrind

OPTIMIZATION_FLAGS=-flto -finline-limit=200
ASORTED_FLAGS=-std=c++11
WARINIG_FLAGS=-Wall -Wextra -pedantic -Werror
FLAGS=$(OPTIMIZATION_FLAGS) $(ASORTED_FLAGS) $(WARNING_FLAGS) $(BUILD_NUMBER_LDFLAGS)
DEBUG_FLAGS=-g -ggdb -pg
NODEBUG_FLAGS=-s -O3 -DNDEBUG

CC=g++ $(FLAGS) $(DEBUG_FLAGS) -I$(INCLUDE)
CCE=g++ $(FLAGS) $(NO_DEBUG_FLAGS) -I$(INCLUDE)

clean:
	rm bin/*

$(HPP): %.h: %.hpp
	@touch bin/$@

$(OBJ): %.o: %.cpp %.h $(patsubst %,%pp,$(HPP))
	@/bin/echo -e "\e[34m  CC $@ \033[0m"
	@$(CC) -c $< -o bin/$@

sais.o:	src/overlap/sais.c
	@/bin/echo -e "\e[34m  CC $@ \033[0m"
	@$(CCE) -fomit-frame-pointer -DNDEBUG  -c -o bin/$@ $^

# build rule for test executables.
$(TEST): %: src/test/%.cpp $(ALL_OBJ)
	@/bin/echo -e "\e[34m  LINK $@ \033[0m"
	@$(CC) $< $(patsubst %,bin/%,$(ALL_OBJ)) -o bin/$@

# build rule for main executables.
# $(EXE): %: src/%.cpp $(ALL_OBJ) $(BUILD_NUMBER_FILE)
# 	@/bin/echo -e "\e[34m  LINK $@ \033[0m"
# 	@$(CC) $< $(patsubst %,bin/%,$(ALL_OBJ)) -o bin/$@
$(EXE): src/$(EXE).cpp $(ALL_OBJ) $(BUILD_NUMBER_FILE)
	@/bin/echo -e "\e[34m  LINK $@ \033[0m"
	@$(CC) $< $(patsubst %,bin/%,$(ALL_OBJ)) -o bin/$@

valgrind: $(EXE)
	valgrind bin/$(EXE)

run: $(EXE) $(BUILD_NUMBER_FILE)
	@bin/$(EXE)
