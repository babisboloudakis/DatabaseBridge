SOURCES = main.cpp read.cpp query.cpp parser.cpp filter.cpp optimize.cpp join.cpp
OBJ = main.o read.o query.o parser.o filter.o optimize.o join.o
PARAMS = -std=c++11

default: $(OBJ)
	@g++ $(OBJ) -o main $(PARAMS)
	
run: default
	@./main

small: default
	@time ./main < input.txt

main.o: main.cpp
	@g++ -c main.cpp $(PARAMS)

read.o: read.cpp ./headers/read.hpp
	@g++ -c read.cpp $(PARAMS)

query.o: query.cpp ./headers/query.hpp
	@g++ -c query.cpp $(PARAMS)

parser.o: parser.cpp ./headers/parser.hpp
	@g++ -c parser.cpp $(PARAMS)

filter.o: filter.cpp ./headers/filter.hpp
	@g++ -c filter.cpp $(PARAMS)

optimize.o: optimize.cpp ./headers/optimize.hpp
	@g++ -c optimize.cpp $(PARAMS)

join.o: join.cpp ./headers/join.hpp
	@g++ -c join.cpp $(PARAMS)

clean:
	@rm -f *.o ./main