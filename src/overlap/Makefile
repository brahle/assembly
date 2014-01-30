#!/usr/bin/env bash
# Directories.
PROJDIR=../..
BDIR=$(PROJDIR)/bin
ODIR=$(PROJDIR)/obj
VPATH=$(PROJDIR)/src:$(PROJDIR)/src/overlap:$(PROJDIR)/src/overlap/test:$(PROJDIR)/src/overlap/wavelet
# Compiler and linker settings.
CXX=g++
# Targets.
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c++11 -I$(PROJDIR)/src -isystem $(PROJDIR)/include -I$(PROJDIR)/src
LDFLAGS=-L$(PROJDIR)/lib -lgflags
# Main sources and object files.
BIN=$(addprefix $(BDIR)/, olapper olapper_test)
SRC=read.cpp fm-index.cpp myers.cpp overlap.cpp sort.cpp suffix-array.cpp \
	suffix-filter.cpp validate.cpp sais.cpp \
	wavelet-tree.cpp ranked-bitmap.cpp
OBJ=$(addprefix $(ODIR)/, $(SRC:.cpp=.o))
# Test sources and object files.
TEST_SRC=read_test.cpp fm-index_test.cpp overlap_test.cpp sort_test.cpp
TEST_OBJ=$(addprefix $(ODIR)/, $(TEST_SRC:.cpp=.o))

all : dev

test : CFLAGS += -DDEBUG -g
test : $(BDIR)/olapper_test
	$(BDIR)/olapper_test

# Build options. Optimized (default/all) or debug.
prod : CFLAGS += -DNDEBUG -O3
prod : $(BIN)

dev : CFLAGS += -DDEBUG -g
dev : $(BIN)

# Linking options.
$(BDIR)/olapper : OBJ := $(ODIR)/main.o $(OBJ)
$(BDIR)/olapper : $(OBJ)

$(BDIR)/olapper_test : LDFLAGS += -lgtest_main -lgtest -lpthread
$(BDIR)/olapper_test : OBJ += $(TEST_OBJ)

# Linking.
$(BIN) : $(OBJ) $(TEST_OBJ) $(ODIR)/main.o
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

# Compiling options.
$(ODIR)/sais.o : CFLAGS += -fomit-frame-pointer

$(ODIR)/%_test.o : CFLAGS += -pthread 

# Compiling.
$(ODIR)/%.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

# Clean up.
clean :
	rm -f $(BDIR)/* $(ODIR)/*
