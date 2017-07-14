# Compiler name
CC = gcc -std=c99 -g -O0 -Wall 
CPP = g++ -g -O0 -Wall 

all: trabalho

CPUTimer.o: CPUTimer.cpp CPUTimer.h
	$(CPP) CPUTimer.cpp -c -o CPUTimer.o

avl.o: avl.c avl.h
	 $(CC) avl.c -c -o avl.o

trabalho.o: trabalho.cpp heap.cpp
	$(CPP) trabalho.cpp -c -o trabalho.o

trabalho: trabalho.o avl.o CPUTimer.o
	$(CPP) trabalho.o avl.o CPUTimer.o -o trabalho

clean:
	rm -f trabalho trabalho.o avl.o CPUTimer.o *~
