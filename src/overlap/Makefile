#!/usr/bin/env bash
# Directories.
PROJDIR=../..
BDIR=$(PROJDIR)/bin
ODIR=$(PROJDIR)/obj
VPATH=$(PROJDIR)/src:$(PROJDIR)/src/overlap:$(PROJDIR)/src/overlap/test
# Compiler and linker settings.
CXX=g++
# Targets.
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c++11
LDFLAGS=-L$(PROJDIR)/lib
# Main sources and object files.
BIN=$(addprefix $(BDIR)/, olapper olapper_test)
SRC=read.cpp fm_index.cpp myers.cpp overlap.cpp \
		sort.cpp suffix_array.cpp suffix_filter.cpp validate.cpp \
		sais.cpp
OBJ=$(addprefix $(ODIR)/, $(SRC:.cpp=.o))
# Test sources and object files.
TEST_SRC=read_test.cpp
TEST_OBJ=$(addprefix $(ODIR)/, $(TEST_SRC:.cpp=.o))

all : dev

test : dev
	$(BDIR)/olapper_test

# Build options. Optimized (default/all) or debug.
prod : CFLAGS += -DNDEBUG -O3
prod : $(BIN)

dev : CFLAGS += -DDEBUG -g
dev : $(BIN)

# Linking options.
$(BDIR)/olapper : OBJ += $(ODIR)/main.o
$(BDIR)/olapper : $(OBJ)

$(BDIR)/olapper_test : LDFLAGS += -lgtest_main -lgtest -lpthread
$(BDIR)/olapper_test : OBJ += $(TEST_OBJ)

# Linking.
$(BIN) : $(OBJ) $(TEST_OBJ) $(ODIR)/main.o
	$(CXX) $(LDFLAGS) -o $@ $(OBJ)

# Compiling options.
$(ODIR)/sais.o : CFLAGS += -fomit-frame-pointer

$(ODIR)/%_test.o : CFLAGS += -pthread -isystem $(PROJDIR)/include

# Compiling.
$(ODIR)/%.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

# Clean up.
clean :
	rm -f $(BDIR)/* $(ODIR)/*
