CC=g++ -pthread -march=native -mtune=native
CFLAGS=-c -g -Ofast -funroll-loops -std=c++0x -Wall -Wextra -Weffc++ -Werror -pedantic \
	-Wdisabled-optimization -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Wcomment -Wconversion \
	-Wfloat-equal -Wformat -Wformat=2 \
	-Wformat-nonliteral -Wformat-security  \
	-Wformat-y2k -Wimport  -Winit-self \
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
	#-Wunsafe-loop-optimizations -Winline #-Wpadded 

all: PatternEvolution TestPatterns GenerateAllPatterns

PatternEvolution: main.o ../UCTNode.o ../UCT.o ../Point.o ../Group.o ../Board.o ../Common.o ../Patterns.o ../Pattern.o
	$(CC) main.o ../UCTNode.o ../UCT.o ../Point.o ../Group.o ../Board.o ../Common.o ../Patterns.o ../Pattern.o -o PatternEvolution -lrt

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

TestPatterns: testPatterns.o ../UCTNode.o ../UCT.o ../Point.o ../Group.o ../Board.o ../Common.o ../Patterns.o ../Pattern.o
	$(CC) testPatterns.o ../UCTNode.o ../UCT.o ../Point.o ../Group.o ../Board.o ../Common.o ../Patterns.o ../Pattern.o -o TestPatterns -lrt

testPatterns.o: testPatterns.cpp
	$(CC) $(CFLAGS) testPatterns.cpp

GenerateAllPatterns: GenerateAllPatterns.o ../Point.o ../Group.o ../Board.o ../Patterns.o ../Pattern.o
	$(CC) GenerateAllPatterns.o ../Point.o ../Group.o ../Board.o ../Patterns.o ../Pattern.o -o GenerateAllPatterns -lrt

GenerateAllPatterns.o: GenerateAllPatterns.cpp
	$(CC) $(CFLAGS) GenerateAllPatterns.cpp

clean:
	rm -rf  *.o PatternEvolution TestPatterns GenerateAllPatterns
