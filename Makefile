NAME = projeto2

all: $(NAME)

$(NAME): projeto2.cpp Makefile
	g++ -std=c++11 -O3 -Wall projeto2.cpp -lm -g -o $(NAME)

clean:
	rm $(NAME)

gera: gera.cpp
	g++ -std=c++11 -O3 -Wall gera.cpp -lm -g -o gerador

cleangerador:
	rm gerador

re: clean all

.PHONY: all clean re gera