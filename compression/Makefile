COMMON	= ../common

DBG      ?=
CXX      ?= g++
CXXFLAGS  = -O3 -I. -std=c++11 -I$(COMMON) $(DBG)
LIBS	= -lbz2

ifeq ($(CXX),icpc)
  CXXFLAGS += -xHost #-no-vec
  CXXFLAGS += -qopt-report=5
  CXXFLAGS += -Wunknown-pragmas # Disable warning about OpenMP pragma no defined.
endif

ifeq ($(CXX),g++)
  CXXFLAGS += -mtune=native -march=native
endif

CXXFLAGS += -fopenmp

EXEC = bzip2

all: $(EXEC)

# Load common make options
include $(COMMON)/Makefile.common
LDFLAGS	  = $(COMMON_LIBS)

bzip2.o: bzip2.cpp
	$(CXX) $(CXXFLAGS) -c bzip2.cpp

bzip2: bzip2.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o bzip2 bzip2.o $(COMMON_OBJS) $(LDFLAGS) $(LIBS)

clean: clean_common
	/bin/rm -fv $(EXEC) *.o *.optrpt *.d
