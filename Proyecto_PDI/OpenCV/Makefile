CXX    = g++
SRCN   = project_opencv
SRC    = $(SRCN).cpp
DIR    = OpenCV
CVLIB  = `pkg-config --cflags --libs opencv`
STDVER = -std=c++11
BINS   = $(shell ls | grep -v '\.cpp' | grep -v '\.png' | grep -v '\Makefile')

all:
		$(CXX) $(SRC) -o $(DIR) $(CVLIB) $(STDVER)

clean:
		rm -f $(BINS)
