SOURCES = main.cpp read.cpp parser.cpp filter.cpp

default: $(SOURCES)
	@g++ $(SOURCES) -o main -std=c++11
	
run: default
	@./main

# main.o: main.cpp
# 	@g++ -c main.cpp -std=c++11

# read.o: read.cpp read.hpp
# 	@g++ -c read.cpp -std=c++11

# parser.o: parser.cpp parser.hpp
# 	@g++ -c parser.cpp -std=c++11

# parser: parser.o
# 	@g++ -o parser paser.o

# filter.o: filter.cpp
# 	@g++ -o filter.cpp -std=c++11

clean:
	@rm -f *.o ./main
