export CPLUS_INCLUDE_PATH=/opt/intel/intel-opencl-1.2-6.3.0.1904/opencl-1.2-sdk-6.3.0.1904/include
export LIBRARY_PATH=/opt/intel/opencl

LIB_SOURCES = $(wildcard lib/*.cpp)
CL_SOURCES  = $(wildcard lib/cl/*.cpp) 
SOURCES=OpenTrader.cpp $(LIB_SOURCES) $(CL_SOURCES)

CL_INC=/usr/include

ifeq ($(CONFIG),debug)
	OPT =-O0 -g
else
	OPT =
endif

all: opentrader_sort

opentrader_sort: $(HEADERS) $(SOURCES) Makefile
	g++ $(SOURCES) -Iinclude -I$(CL_INC) -lOpenCL -oOpenTrader -std=gnu++0x $(OPT)

clean:
	rm -f OpenTrader

