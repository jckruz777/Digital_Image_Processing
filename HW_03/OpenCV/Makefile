CXX    = g++
SRCN   = opencv_t03
SRC    = $(SRCN).cpp
CVLIB  = `pkg-config --cflags --libs opencv`
BINS   = $(shell ls | grep -v '\.cpp' | grep -v '\.txt' | grep -v '\Makefile')

all:
		$(CXX) $(SRC) -o $(SRCN) $(CVLIB)

clean:
		rm -f $(BINS)
