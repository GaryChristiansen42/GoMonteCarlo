CC=g++
CFLAGS=-c -Wall -Wextra -Weffc++ -Werror -pedantic \
	-Wdisabled-optimization -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Wcomment -Wconversion \
	-Wfloat-equal -Wformat -Wformat=2 \
	-Wformat-nonliteral -Wformat-security  \
	-Wformat-y2k -Wimport  -Winit-self  -Winline \
	-Winvalid-pch -Wunsafe-loop-optimizations  \
	-Wlong-long -Wmissing-braces -Wmissing-field-initializers \
	-Wmissing-format-attribute -Wmissing-include-dirs \
	-Wmissing-noreturn -Wpacked  -Wpadded -Wparentheses  \
	-Wpointer-arith -Wredundant-decls -Wreturn-type \
	-Wsequence-point  -Wshadow -Wsign-compare  -Wstack-protector \
	-Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch  -Wswitch-default \
	-Wswitch-enum -Wtrigraphs  -Wuninitialized \
	-Wunknown-pragmas  -Wunreachable-code -Wunused \
	-Wunused-function  -Wunused-label  -Wunused-parameter \
	-Wunused-value  -Wunused-variable  -Wvariadic-macros \
	-Wvolatile-register-var  -Wwrite-strings \
  -g

all: GoMonteCarlo CheckStrength

CheckStrength: CheckStrength.o UCTNode.o UCT.o Point.o Group.o Board.o
	$(CC) CheckStrength.o UCTNode.o UCT.o Point.o Group.o Board.o -o CheckStrength

CheckStrength.o: CheckStrength.cpp
	$(CC) $(CFLAGS) CheckStrength.cpp

GoMonteCarlo: main.o UCTNode.o UCT.o Point.o Group.o Board.o
	$(CC) main.o UCTNode.o UCT.o Point.o Group.o Board.o -o GoMonteCarlo

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

UCTNode.o: UCTNode.cpp
	$(CC) $(CFLAGS) UCTNode.cpp

UCT.o: UCT.cpp
	$(CC) $(CFLAGS) UCT.cpp
	
Point.o: Point.cpp
	$(CC) $(CFLAGS) Point.cpp

Group.o: Group.cpp
	$(CC) $(CFLAGS) Group.cpp

Board.o: Board.cpp
	$(CC) $(CFLAGS) Board.cpp

clean:
	rm -rf  *.o GoMonteCarlo
