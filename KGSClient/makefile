CC=g++ -march=native -mtune=native
CFLAGS=-c -g -std=c++0x -Ofast -funroll-loops -Wall -Wextra -Weffc++ -Werror -pedantic \
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
	-Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch  -Wswitch-default \
	-Wswitch-enum -Wtrigraphs  -Wuninitialized \
	-Wunknown-pragmas  -Wunreachable-code -Wunused \
	-Wunused-function  -Wunused-label  -Wunused-parameter \
	-Wunused-value  -Wunused-variable  -Wvariadic-macros \
	-Wvolatile-register-var  -Wwrite-strings \
	#-Wpadded -Wunsafe-loop-optimizations 

GTPClient: main.o UCTNode.o UCT.o Point.o Group.o Board.o Common.o Patterns.o Pattern.o Pattern3x3.o Pattern5x5.o
	$(CC) main.o UCTNode.o UCT.o Point.o Group.o Board.o Common.o Patterns.o Pattern.o Pattern3x3.o Pattern5x5.o -o GTPClient -lrt 

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

Pattern3x3.o: ../Pattern3x3.cpp
	$(CC) $(CFLAGS) ../Pattern3x3.cpp

Pattern5x5.o: ../Pattern5x5.cpp
	$(CC) $(CFLAGS) ../Pattern5x5.cpp

Pattern.o: ../Pattern.cpp
	$(CC) $(CFLAGS) ../Pattern.cpp

Patterns.o: ../Patterns.cpp
	$(CC) $(CFLAGS) ../Patterns.cpp

clean:
	rm -rf  *.o GTPClient
