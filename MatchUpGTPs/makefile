CC=g++
CFLAGS=-c -g -std=c++0x -O0 -Wall -Wextra -Weffc++ -Werror -pedantic \
	-Wdisabled-optimization -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Wcomment -Wconversion \
	-Wfloat-equal -Wformat -Wformat=2 \
	-Wformat-nonliteral -Wformat-security  \
	-Wformat-y2k -Wimport  -Winit-self  -Winline \
	-Winvalid-pch -Wlong-long -Wmissing-braces \
  -Wmissing-field-initializers \
	-Wmissing-format-attribute -Wmissing-include-dirs \
	-Wmissing-noreturn -Wpacked  -Wparentheses  \
	-Wpointer-arith -Wredundant-decls -Wreturn-type \
	-Wsequence-point  -Wshadow -Wsign-compare  -Wstack-protector \
	-Wswitch  -Wswitch-default \
	-Wswitch-enum -Wtrigraphs  -Wuninitialized \
	-Wunknown-pragmas  -Wunreachable-code -Wunused \
	-Wunused-function  -Wunused-label  -Wunused-parameter \
	-Wunused-value  -Wunused-variable  -Wvariadic-macros \
	-Wvolatile-register-var  -Wwrite-strings \
	#-Wpadded -Wunsafe-loop-optimizations 

all: MatchUpGTPs GTPNetworkClient

MatchUpGTPs: MatchUpGTPs.o UCTNode.o UCT.o Point.o Group.o Board.o Common.o
	$(CC) MatchUpGTPs.o UCTNode.o UCT.o Point.o Group.o Board.o Common.o -o MatchUpGTPs -lrt

GTPNetworkClient: GTPNetworkClient.o UCTNode.o UCT.o Point.o Group.o Board.o Common.o
	$(CC) GTPNetworkClient.o UCTNode.o UCT.o Point.o Group.o Board.o Common.o -o GTPNetworkClient -lrt

MatchUpGTPs.o: MatchUpGTPs.cpp
	$(CC) $(CFLAGS) MatchUpGTPs.cpp

GTPNetworkClient.o: GTPNetworkClient.cpp
	$(CC) $(CFLAGS) GTPNetworkClient.cpp

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

clean:
	rm -rf  *.o MatchUpGTPs GTPNetworkClient
