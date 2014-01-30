#!/usr/bin/env bash
# Directories.
PROJDIR=../..
VPATH=$(PROJDIR)/src:$(PROJDIR)/src/overlap:$(PROJDIR)/src/overlap/tests
ODIR=$(PROJDIR)/obj
BDIR=$(PROJDIR)/bin
# Compiler and linker settings.
CXX=g++
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c++11 -isystem $(PROJDIR)/include
LDFLAGS=-L$(PROJDIR)/lib
# Targets.
BIN=olapper olapper_test
# Main sources and object files.
SRC=read.cpp fm_index.cpp myers.cpp overlap.cpp \
		sort.cpp suffix_array.cpp suffix_filter.cpp validate.cpp \
		sais.cpp
OBJ=$(SRC:.cpp=.o)
# Test sources and object files.
TEST_SRC=
TEST_OBJ=$(TEST_SRC:.cpp=.o)
# Debug object file/binary prefix.
PRE=

# Build options. Optimized (default/all) or debug.
all : CFLAGS += -DNDEBUG -O3
all : $(BIN)

debug : CFLAGS += -DDEBUG -g
debug : PRE = debug_
debug : $(BIN)

# Linking options.
olapper : OBJ += main.o
olapper : $(OBJ)

olapper_test : LDFLAGS += -lgtest_main -lgtest -lpthread
olapper_test : OBJ += $(TEST_OBJ)

# Linking.
$(BIN) : $(OBJ) $(TEST_OBJ) main.o
	$(CXX) $(LDFLAGS) -o $(BDIR)/${PRE}$@ $(addprefix $(ODIR)/, $(OBJ))

# Compiling options.
sais.o : CFLAGS += -fomit-frame-pointer

%_test.o : CFLAGS += -pthread

# Compiling.
%.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $(ODIR)/$(PRE)$@ $<

# Clean up.
clean :
	rm -f $(BDIR)/* $(ODIR)/*
