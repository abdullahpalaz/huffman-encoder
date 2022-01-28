output: main.o
	g++ main.o -o Main

main.o: main.cpp
	g++ -c main.cpp