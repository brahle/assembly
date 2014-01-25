#!/usr/bin/env bash
CXX=g++
CFLAGS=-Wall -Wextra -Werror -pedantic -O2 -std=c++11
LDFLAGS=-lsais
# Sources.
SRCDIR=src
SOURCES=main.cpp read.cpp fm_index.cpp overlap.cpp sort.cpp suffix_array.cpp suffix_filter.cpp
# Objects.
OBJDIR=obj
OBJECTS=$(SOURCES:.cpp=.o)
# Binaries.
BINDIR=bin
BINARIES=olapper

all : $(addprefix $(SRCDIR)/, $(SOURCES)) $(BINARIES)

$(BINARIES) : $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(CXX) $^ -o $(BINDIR)/$@ $(LDFLAGS)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $< -c -o $@

clean :
	rm -f $(BINDIR)/* $(OBJDIR)/*
