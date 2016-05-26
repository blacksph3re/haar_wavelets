build:
	g++ main2.cpp -o haar -std=c++14 -O3 -fopenmp
	
run:
	OMP_NUM_THREADS=4 ./haar input output
	
scalingtest:
	OMP_NUM_THREADS=1 ./haar input output	
	OMP_NUM_THREADS=2 ./haar input output	
	OMP_NUM_THREADS=4 ./haar input output	
	OMP_NUM_THREADS=8 ./haar input output
	OMP_NUM_THREADS=16 ./haar input output	
	OMP_NUM_THREADS=32 ./haar input output	
