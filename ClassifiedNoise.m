# makefile for ClassifiedNoise exploration of the Numenta NuPIC code
CC=g++
LINK=g++
CFLAGS= -c -std=gnu++11 -I $(NUPIC_CORE)/src
LFLAGS= -std=gnu++11
LIBS= $(NUPIC_CORE)/lib/_algorithms.so  $(NUPIC_CORE)/lib/_engine_internal.so

OBJS=ClassifiedNoise.o CmdLineParse.o SpParamFile.o BufferSummary.o Sparse.o Paragon.o NoiseSource.o
ICOMMON=

all: ClassifiedNoise

ClassifiedNoise.o : ClassifiedNoise.cpp
	$(CC) $(CFLAGS) $*.cpp

CmdLineParse.o : CmdLineParse.cpp
	$(CC) $(CFLAGS) $*.cpp


ClassifiedNoise:	$(OBJS)
	$(LINK) $(LFLAGS) -o ClassifiedNoise $(OBJS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $*.cpp

clean:
	rm *.o
	rm ClassifiedNoise

