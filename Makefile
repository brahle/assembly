CXX=g++
CFLAGS=-c -Wall -O2 -std=c++0x
LDFLAGS=-lsais
SOURCES=main.cpp data.cpp fm_index.cpp suffix_array.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=olapper

all : $(addprefix src/, $(SOURCES)) $(EXECUTABLE)

$(EXECUTABLE) : $(addprefix obj/, $(OBJECTS))
	$(CXX) $^ -o bin/$@ $(LDFLAGS)

obj/%.o : src/%.cpp
	$(CXX) $(CFLAGS) $< -o $@
