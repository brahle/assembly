default: run

INCLUDE=./src
VPATH=src:src/layout:src/overlap:bin

# these headers are different. first, they don't exists
# but *.hpp files exists. it means that they contain template
# implementation and everything must be recompiled when
# they are changed.
HPP=

OBJ=union_find.o unitigging.o data.o

TEST=

EXE=main

include buildnumber.mak

OPTIMIZATION_FLAGS=-flto -finline-limit=200
ASORTED_FLAGS=-std=c++11
WARINIG_FLAGS=-Wall -Wextra -pedantic -Werror
FLAGS=$(OPTIMIZATION_FLAGS) $(ASORTED_FLAGS) $(WARNING_FLAGS) $(BUILD_NUMBER_LDFLAGS)
DEBUG_FLAGS=-g -ggdb -pg
NODEBUG_FLAGS=-s -O3

CC=g++ $(FLAGS) $(DEBUG_FLAGS) -I$(INCLUDE)
CCE=g++ $(FLAGS) $(NO_DEBUG_FLAGS) -I$(INCLUDE)

clean:
	rm bin/*

$(HPP): %.h: %.hpp
	@touch bin/$@

$(OBJ): %.o: %.cpp %.h $(patsubst %,%pp,$(HPP))
	@/bin/echo -e "\e[34m  CC $@ \033[0m"
	@$(CC) -c $< -o bin/$@

# build rule for test executables.
$(TEST): %: src/test/%.cpp $(OBJ)
	@/bin/echo -e "\e[34m  LINK $@ \033[0m"
	@$(CC) $< $(patsubst %,bin/%,$(OBJ)) -o bin/$@

# build rule for main executables.
$(EXE): %: src/%.cpp $(OBJ)
	@/bin/echo -e "\e[34m  LINK $@ \033[0m"
	@$(CC) $< $(patsubst %,bin/%,$(OBJ)) -o bin/$@

valgrind: $(EXE)
	valgrind bin/$(EXE)

run: $(EXE)
	@bin/$(EXE)
