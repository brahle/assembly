#!/usr/bin/env bash
CXX=g++
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c++11
LDFLAGS=
# Sources.
SRCDIR=.
SOURCES=main.cpp read.cpp fm_index.cpp myers.cpp overlap.cpp \
		sort.cpp suffix_array.cpp suffix_filter.cpp validate.cpp
# Objects.
OBJDIR=../../obj
OBJECTS=$(SOURCES:.cpp=.o) sais.o
# Binaries.
BINDIR=../../bin
BINARIES=olapper


all : CFLAGS += -O3
all : $(addprefix $(SRCDIR)/, $(SOURCES)) $(BINARIES)

debug: CFLAGS += -DDEBUG -g
debug: $(BINARIES)

$(BINARIES) : $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(CXX) $^ -o $(BINDIR)/$@ $(LDFLAGS)

$(OBJDIR)/sais.o : $(SRCDIR)/sais.c
	$(CXX) $(CFLAGS) -fomit-frame-pointer -DNDEBUG  -c -o $@ $^

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $< -c -o $@

clean :
	rm -f $(BINDIR)/* $(OBJDIR)/*
