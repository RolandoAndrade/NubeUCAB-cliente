all:
	g++ -c main.cpp -o main.o
	g++ main.o -o nuclient
	rm main.o

clean:
	rm nuclient
