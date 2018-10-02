CXX_FLAGS = -std=c++11 -Os
LD_FLAGS = -lncurses

all: run

main.o: main.cpp
	clang++ ${CXX_FLAGS} -o main.o -c main.cpp

board.o: board.cpp
	clang++ ${CXX_FLAGS} -o board.o -c board.cpp

cell.o: cell.cpp
	clang++ ${CXX_FLAGS} -o cell.o -c cell.cpp

cli.o: cli.cpp
	clang++ ${CXX_FLAGS} -o cli.o -c cli.cpp

sko: cell.o board.o cli.o main.o Makefile
	clang++ -o sko ${LD_FLAGS} cell.o board.o cli.o main.o

run: sko
	./sko

clean:
	rm -f ./*.o

install:
	echo "ok"