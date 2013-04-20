GPP = g++
GCC = gcc

COMPILE_FLAGS = -c -m32 -O3 -fPIC -w -DLINUX -Wall -I src/SDK/amx/

OUTFILE = "bin/timerfix.so"

all:
	$(GPP) $(COMPILE_FLAGS) src/SDK/*.cpp
	$(GPP) $(COMPILE_FLAGS) src/*.cpp
	$(GPP) -fshort-wchar -shared -o $(OUTFILE) *.o -lrt
	rm -f *.o