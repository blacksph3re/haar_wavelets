build:
	g++ main2.cpp -o haar -std=c++14 -O3 -fopenmp

run:
	OMP_NUM_THREADS=4 ./haar input output
