all:
	g++ -std=c++11 -O3 -Wall projeto2.cpp -lm -g

clean:
	rm a.out

gera:
	g++ -std=c++11 -O3 -Wall gera.cpp -lm -g -o gerador

cleangerador:
	rm gerador