default: main.o
	@g++ -o main main.o
	
main.o: main.cpp
	@g++ -c main.cpp -std=c++11

run: default
	@./main
read: read.cpp
	@g++ read.cpp -o read -std=c++11
read_run:
	@./read
clean:
	@rm -f *.o ./main ./read
