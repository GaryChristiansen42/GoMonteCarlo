CC=g++
CFLAGS=-c -O -Wall -Wextra -Werror -pedantic -g

all: TestGoMonteCarlo

TestGoMonteCarlo: main.o UCTNode.o UCT.o Point.o Group.o Board.o
	$(CC) main.o UCTNode.o UCT.o Point.o Group.o Board.o -o TestGoMonteCarlo

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

clean:
	rm -rf  *.o TestGoMonteCarlo
