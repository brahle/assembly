#!/usr/bin/env bash
CXX=g++
CFLAGS=-c -Wall -O2 -std=c++0x
LDFLAGS=-lsais
# Sources.
SRCDIR=src
SOURCES=main.cpp data.cpp fm_index.cpp suffix_array.cpp
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
	$(CXX) $(CFLAGS) $< -o $@

clean :
	rm -f $(BINDIR)/* $(OBJDIR)/*
