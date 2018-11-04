default: main.o
	@g++ -o main main.o
	
main.o: main.cpp
	@g++ -c main.cpp -std=c++11

run: default
	@./main
clean:
	@rm -f *.o ./main
