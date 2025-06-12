COMPILER = g++
FLAGS = -O3 -march=native -funroll-loops -fprefetch-loop-arrays -Wall -g
SOURCEFILE = main.cpp
OUTPUT = pmedian

$(OUTPUT): $(SOURCEFILE)
	$(COMPILER) $(FLAGS) $(SOURCEFILE) -o $(OUTPUT)
