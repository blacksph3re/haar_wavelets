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

inputfiles:
	gcc input_generator.c -o input_generator
	./input_generator input
	rm input_generator
	
outputfiles:
	gcc haar.c -o haar_seq -O3
	./haar_seq input output
	rm haar_seq
