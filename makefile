#compiler
CC = g++

CFLAGS = -g -Wall -c

compile : TimeSynchronization

TimeSynchronization: TimeSynchronization.o
	g++ TimeSynchronization.o -o TimeSynchronization -lpthread

TimeSynchronization.o: TimeSynchronization.cpp
	g++ -c -lpthread TimeSynchronization.cpp 

clean:
	rm -rf *.o compile
