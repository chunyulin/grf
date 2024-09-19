CXX=icpx
TARGET=grf

LIB = -L/usr/lib/x86_64-linux-gnu -lpng
CXXFLAGS=-O2 -pg --std=c++11

COMMON=--std=c++11 -pg
##-march=sapphirerapids -qopenmp
##-march=native|sapphirerapids

all: $(TARGET)

testO: grf.cpp utils.cpp
	$(CXX) -O0    -o grfO0 $^ $(LIB)  $(COMMON)
	$(CXX) -O1    -o grfO1 $^ $(LIB)  $(COMMON)
	$(CXX) -O2    -o grfO2 $^ $(LIB)  $(COMMON)
	$(CXX) -O3    -o grfO3 $^ $(LIB)  $(COMMON)
	$(CXX) -Ofast -o grfOf $^ $(LIB)  $(COMMON)

verbose:
	g++ -O3    -o grf grf.cpp utils.cpp $(LIB) -pg --std=c++11 -ftree-vectorizer-verbose=n


grf: grf.cpp utils.cpp
	$(CXX) -o $@ $^ $(LIB) $(CXXFLAGS)

clean:
	rm *.o -f ${TARGET}

.SUFFIXES : .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -c -o $@ $(DEBUG)


