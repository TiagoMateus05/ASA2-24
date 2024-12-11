all:
	g++ -std=c++11 -O3 -Wall projeto2.cpp -lm
	./a.out input1.in

clean:
	rm a.out
