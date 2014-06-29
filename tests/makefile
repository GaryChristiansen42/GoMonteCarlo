CC=g++ -pthread
CFLAGS=-c -g -O0 --std=c++0x -Wall -Wextra -Werror -pedantic

all: TestGoMonteCarlo

TestGoMonteCarlo: main.o UCTNode.o UCT.o Point.o Group.o Board.o Common.o Patterns.o Pattern.o
	$(CC) main.o UCTNode.o UCT.o Point.o Group.o Board.o Common.o Patterns.o Pattern.o -o TestGoMonteCarlo -lrt

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

UCTNode.o: ../UCTNode.cpp
	$(CC) $(CFLAGS) ../UCTNode.cpp

UCT.o: ../UCT.cpp
	$(CC) $(CFLAGS) ../UCT.cpp
	
Point.o: ../Point.cpp
	$(CC) $(CFLAGS) ../Point.cpp

Group.o: ../Group.cpp
	$(CC) $(CFLAGS) ../Group.cpp

Board.o: ../Board.cpp
	$(CC) $(CFLAGS) ../Board.cpp

Common.o: ../Common.cpp
	$(CC) $(CFLAGS) ../Common.cpp

Pattern.o: ../Pattern.cpp
	$(CC) $(CFLAGS) ../Pattern.cpp

Patterns.o: ../Patterns.cpp
	$(CC) $(CFLAGS) ../Patterns.cpp

clean:
	rm -rf  *.o TestGoMonteCarlo
