CXX=g++
TARGET= grf

PNG_LIB = -L/usr/lib/x86_64-linux-gnu -lpng
CXXFLAGS=-Ofast -std=c++11

all: $(TARGET)


grf: grf.o utils.o
	$(CXX) -o $@ $^ $(PNG_LIB) $(CXXFLAGS)

grfn: grfn.o utils.o
	$(CXX) -o $@ $^ $(PNG_LIB) $(CXXFLAGS) -pg


nsys:
	export CUDA_VISIBLE_DEVICES=0; \
           ${NSYS} profile -o nuosc -f true -t cuda,nvtx \
           ./nuosc --ipt 0 --pmo 1e-5 --mu 1 --ko 1e-3 --zmax 1024 --dz 0.05 --nv 400 --cfl 0.5 --alpha 0.9 --eps0 1e-1 --sigma 2 \
              --ANA_EVERY 999 --DUMP_EVERY 999 --END_STEP 10
ncu:
	${NCU} --nvtx -f --set full --sampling-interval 6 -o FD \
           ./nuosc --ipt 0 --mu 1.0 --zmax 1024 --dz 0.05 --nv 400 --cfl 0.5 --ko 1e-3 --ANA_EVERY 9999 --DUMP_EVERY 9999 --END_STEP 5


clean:
	rm *.o -f ${TARGET}

.SUFFIXES : .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@ $(DEBUG)


