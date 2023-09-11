# Basic makefile to compile the memory sim program with each trace and page selection algorithim

# E.g. Run "make clock2" to compile and run the memsim with trace 2 and the clock algorithim
# Run "make compile" to just compile the memsim program
# Run "make" to run every combination of trace file and algorithim in order

compile: memsim.c
	gcc -Wall -O3 -o memsim memsim.c
#	gcc -o memsim memsim.c

all: clock1 lru1 clock2 lru2 clock3 lru3

clock1: compile memsim
	./memsim trace1 4 clock debug
	bash -c 'diff -u trace1-4frames-clock <(./memsim trace1 4 clock quiet)'

lru1: compile memsim
	./memsim trace1 4 lru debug
	bash -c 'diff -u trace1-4frames-lru <(./memsim trace1 4 lru quiet)'

clock2: compile memsim
	./memsim trace2 6 clock debug
	bash -c 'diff -u trace2-6frames-clock <(./memsim trace2 6 clock quiet)'

lru2: compile memsim
	./memsim trace2 6 lru debug
	bash -c 'diff -u trace2-6frames-lru <(./memsim trace2 6 lru quiet)'

clock3: compile memsim
	./memsim trace3 4 clock debug
	bash -c 'diff -u trace3-4frames-clock <(./memsim trace3 4 clock quiet)'

lru3: compile memsim
	./memsim trace3 4 lru debug
	bash -c 'diff -u trace3-4frames-lru <(./memsim trace3 4 lru quiet)'

test: compile memsim
	./memsim $(file) $(frames) $(algo) quiet > results/$(file)_$(frames)_$(algo).txt



