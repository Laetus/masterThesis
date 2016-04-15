# Copyright (C) 2005, 2010 International Business Machines and others.
# All Rights Reserved.
# This file is distributed under the Eclipse Public License.

# $Id: Makefile.in 2016 2011-06-15 08:53:32Z stefan $

##########################################################################
#    You can modify this example makefile to fit for your own program.   #
#    Usually, you only need to change the five CHANGEME entries below.   #
##########################################################################

# CHANGEME: This should be the name of your executable build/vdp_multiple_shooting.exe 
EXE = build/Global_Realtime.echse

# CHANGEME: Here is the name of all object files corresponding to the source
#           code that you wrote in order to define the problem statement  tutorials/vdp_multiple_shooting.o 
OBJS =  Global_Realtime.o
	

# CHANGEME: Additional libraries
ADDLIBS =  /usr/local/lib/libcasadi.so -L/home/laetus/.PLplot/install/lib \
			-Wl,-rpath,/home/laetus/.PLplot/install/lib -lplplot -lplplotcxx -fopenmp \
			-I /usr/local/boost_1_60_0
			

# CHANGEME: Additional flags for compilation (e.g., include flags)
ADDINCFLAGS = -std=c++11 -ggdb -I/home/laetus/.PLplot/install/include/plplot -fopenmp \
				-I /usr/local/boost_1_60_0

##########################################################################
#  Usually, you don't have to change anything below.  Note that if you   #
#  change certain compiler options, you might have to recompile Ipopt.   #
##########################################################################

# C++ Compiler command
CXX = g++

# C++ Compiler options
CXXFLAGS =  -O3 -pipe -DNDEBUG  -pedantic-errors -Wparentheses -Wreturn-type -Wcast-qual -Wall -Wpointer-arith -Wwrite-strings -Wconversion -Wno-unknown-pragmas -Wno-long-long -DIPOPT_BUILD 

# additional C++ Compiler options for linking
CXXLINKFLAGS =  -Wl,--rpath -Wl,/home/laetus/.CoinIpopt/Ipopt/lib

# Include directories (we use the CYGPATH_W variables to allow compilation with Windows compilers)
INCL = `PKG_CONFIG_PATH=/home/laetus/.CoinIpopt/Ipopt/lib64/pkgconfig:/home/laetus/.CoinIpopt/Ipopt/lib/pkgconfig:/home/laetus/.CoinIpopt/Ipopt/share/pkgconfig: pkg-config --cflags ipopt` $(ADDINCFLAGS)  
#INCL = -I`$(CYGPATH_W) /home/laetus/.CoinIpopt/Ipopt/include/coin`  $(ADDINCFLAGS)

# Linker flags
LIBS = `PKG_CONFIG_PATH=/home/laetus/.CoinIpopt/Ipopt/lib64/pkgconfig:/home/laetus/.CoinIpopt/Ipopt/lib/pkgconfig:/home/laetus/.CoinIpopt/Ipopt/share/pkgconfig: pkg-config --libs ipopt`  
##LIBS = -link -libpath:`$(CYGPATH_W) /home/laetus/.CoinIpopt/Ipopt/lib` libipopt.lib -llapack -lblas -lm  -ldl
#LIBS = -L/home/laetus/.CoinIpopt/Ipopt/lib -lipopt -llapack -lblas -lm  -ldl

# The following is necessary under cygwin, if native compilers are used
CYGPATH_W = echo

all: clean $(EXE)

.SUFFIXES: .cpp .c .o .obj

$(EXE): $(OBJS)
	
	bla=;\
	for file in $(OBJS); do bla="$$bla `$(CYGPATH_W) $$file`"; done; \
	$(CXX) $(CXXLINKFLAGS) $(CXXFLAGS) -o $@ $$bla $(ADDLIBS) $(LIBS)

clean:
	rm -rf $(EXE) $(OBJS) ipopt.out

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCL) -c -o $@ $< 


.cpp.obj:
	$(CXX) $(CXXFLAGS) $(INCL) -c -o $@ `$(CYGPATH_W) '$<'`