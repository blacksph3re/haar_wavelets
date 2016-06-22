#!/bin/sh

for i in 1 2 4 8 12 16 24 32 
do
	echo "------------"
	echo "Number of threads = $i"
	export OMP_NUM_THREADS=$i
	time ./${EXECUTABLE=haar} input output/output
done
