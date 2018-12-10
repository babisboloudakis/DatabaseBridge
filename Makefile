SOURCES = main.cpp read.cpp parser.cpp filter.cpp
OBJ = main.o read.o parser.o filter.o
PARAMS = -std=c++11

default: $(OBJ)
	@g++ $(OBJ) -o main $(PARAMS)
	
run: default
	@./main

small: default
	@time ./main < input.txt

main.o: main.cpp
	@g++ -c main.cpp $(PARAMS)

read.o: read.cpp read.hpp
	@g++ -c read.cpp $(PARAMS)

parser.o: parser.cpp parser.hpp
	@g++ -c parser.cpp $(PARAMS)

filter.o: filter.cpp
	@g++ -c filter.cpp $(PARAMS)

clean:
	@rm -f *.o ./main