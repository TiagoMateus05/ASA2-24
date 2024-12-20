#!/bin/bash

# Compile the necessary programs
g++ -std=c++11 -O3 -Wall gera.cpp -lm -g -o gerador
g++ -std=c++11 -O3 -Wall linear_regression.cpp -lm -g -o linear_regression
# Navigate to the project directory

g++ -std=c++11 -O3 -Wall projeto.cpp -lm -g -o projeto

[ -f output.txt ] && rm output.txt

# Run the gerador executable 50 times with random n and m values and save the output to .in files
for i in $(seq 1 3000); do

    #Inputs Maximos dos Privados
    #n=$((1 + RANDOM % 50000))
    #m=$((1 + RANDOM % 5000000)) 
    #l=$((1 + RANDOM % 100))

    n=$((2 + RANDOM % 500000000000000000000))
    m=$((1 + RANDOM % ($n - 1)))
    l=$((1 + RANDOM % 100 % $m))
    echo "Generating Input: input_$i"
    # Run the generator and save output
    ./gerador $n $m $l> "input_${i}.in"
done

# Process files 1 to 50
for i in $(seq 1 3000); do
    input_file="input_${i}.in"
    base_name=$(basename "$input_file" .in)

    echo "Processing File: "$input_file""
    # Run the program with the input file
    echo "Input file: ${base_name}:" >> output.txt
    read n m l< "$input_file"
    result=$((n + m + m*l*l + n*l*l))
    echo "Value n: $n" >> output.txt
    echo "Value m: $m" >> output.txt
    echo "Value l: $l" >> output.txt
    echo "Complexity Value: O($result)" >> output.txt
    # Capture time taken by the program and redirect output
    ./projeto < "$input_file" >> output.txt
    echo "" >> output.txt
done

for input_file in *.in; do
    rm "$input_file"
done

./linear_regression
