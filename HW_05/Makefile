CXX    = g++
SRCN   = tarea05
SRC    = $(SRCN).cpp
CVLIB  = `pkg-config --cflags --libs opencv`
STDVER = -std=c++11
BINS   = $(shell ls | grep -v '\.cpp' | grep -v '\.txt' | grep -v '\.jpg' | grep -v '\Makefile')

all:
		$(CXX) $(SRC) -o $(SRCN) $(CVLIB) $(STDVER)

clean:
		rm -f $(BINS)
