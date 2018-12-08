default: main.o read.o
	@g++ -o main main.o read.o
	
main.o: main.cpp
	@g++ -c main.cpp -std=c++11

run: default
	@./main

read.o: read.cpp read.hpp
	@g++ -c read.cpp -std=c++11

parser.o: parser.cpp parser.hpp
	@g++ -c parser.cpp -std=c++11

parser: parser.o
	@g++ -o parser paser.o

clean:
	@rm -f *.o ./main ./read
