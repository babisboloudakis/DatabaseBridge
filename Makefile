OBJ = main.o read.o query.o parser.o filter.o optimize.o join.o scheduler.o
PARAMS = -std=c++11 -lpthread -O3

default: $(OBJ)
	@g++ $(OBJ) -o main $(PARAMS)

harness:
	@g++ -o harness harness.cpp -std=c++11

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

scheduler.o: scheduler.cpp ./headers/scheduler.hpp
	@g++ -c scheduler.cpp -pthread $(PARAMS)

clean:
	@rm -f *.o ./main