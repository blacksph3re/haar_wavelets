build:
	${CXX} main.cpp -o haar -std=c++14 -O3 -fopenmp 
	
mmap:
	${CXX} main_mmap.cpp -o haar_mmap -std=c++14 -O3 -fopenmp -lboost_iostreams


ctime:
	${CXX} main.cpp -o haar_ctime -std=c++14 -O3 -fopenmp -DCTIME

	
run:
	OMP_NUM_THREADS=4 ./haar input output/output
	
scalingtest:
	OMP_NUM_THREADS=1 ./haar input output/output1
	OMP_NUM_THREADS=2 ./haar input output/output2	
	OMP_NUM_THREADS=4 ./haar input output/output3	
	OMP_NUM_THREADS=8 ./haar input output/output4
	OMP_NUM_THREADS=16 ./haar input output/output5	
	OMP_NUM_THREADS=32 ./haar input output/output6	

inputfiles:
	gcc input_generator.c -o input_generator
	./input_generator input
	rm input_generator
	
outputfiles:
	gcc haar.c -o haar_seq -O3
	./haar_seq input output
	rm haar_seq
	
clean:
	rm haar haar_ctime haar_mmap
	#rm input
	rm output/*
