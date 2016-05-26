#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <omp.h>
#include <boost/iostreams/device/mapped_file.hpp>

//#define CTIME

#ifdef CTIME
#include <ctime>
auto to_millis(auto input) {
	return input/1000;
}

auto myclock() {
	return clock();
}

#else
#include <chrono>
long int to_millis(auto input) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(input).count();
}

std::chrono::system_clock::time_point myclock() {
	return std::chrono::system_clock::now();
}
#endif

void process(int32_t* pixels, int32_t size) {
	auto pxl = [&](unsigned int x, unsigned int y) -> int32_t& {return pixels[x+y*size];};
	
	for (auto s = size; s > 1; s /= 2) {
		auto mid = s/2;
				
		// row-transform
		#pragma omp parallel for
		for (auto y = 0; y < mid; ++y) {
			for (auto x = 0; x < mid; ++x) {
				auto tmp1 = pxl(x, y);
				auto tmp2 = pxl(x+mid, y);
				pxl(x, y) = (tmp1 + tmp2) / M_SQRT2;
				pxl(x+mid, y) = (tmp1 - tmp2) / M_SQRT2;
			}
		}
				
		// col-transform
		#pragma omp parallel for
		for (auto y = 0; y < mid; y++) {
			for (auto x = 0; x < mid; x++) {
				auto tmp1 = pxl(x, y);
				auto tmp2 = pxl(x, y+mid);
				pxl(x, y) = (tmp1 + tmp2) / M_SQRT2;
				pxl(x, y+mid) = (tmp1 - tmp2) / M_SQRT2;
			}
		}
	}
}

int main(int argc, char* argv[]) {
	std::ifstream ifile;
	boost::iostreams::mapped_file_sink ofile;
	
	if (argc != 3) {
        std::cout << "usage: " << argv[0] << " [input_file] [output_file]" << std::endl;
        return 255;
    }

    auto begin = myclock();

    // Open the files
    ifile.open(argv[1], std::ios::binary | std::ios::in);
	if(!ifile.is_open()) {
		std::cout << "couldn't open input file" << std::endl;
		return -1;
	}
	
	// Read the data into the mmap
	int32_t size;
	ifile.read(reinterpret_cast<char*>(&size), sizeof(size));	
	
	boost::iostreams::mapped_file_params params(argv[2]);
	params.length = (size*size+1)*sizeof(int32_t);
	params.new_file_size = (size*size+1)*sizeof(int32_t);
	ofile.open(params);
	if(!ofile.is_open()) {
		std::cout << "couldn't open output file" << std::endl;
		return -1;
	}
	int32_t* pixels = reinterpret_cast<int32_t*>(ofile.data());
	pixels[0] = size;
	pixels += sizeof(int32_t);

	ifile.read(reinterpret_cast<char*>(pixels), size*size*sizeof(int32_t));
	ifile.close();
	
	auto after_reading = myclock();
	std::cout << "Reading took " << to_millis(after_reading - begin) << "ms " << std::endl;
	std::cout << "Processing an image of " << size << "x" << size << " dimensions with up to " << omp_get_max_threads()  << " threads" << std::endl;
	
	process(pixels, size);
	auto after_processing = myclock();
	
	std::cout << "Processing took " << to_millis(after_processing - after_reading) << "ms " << std::endl;
	
	//ofile.write(reinterpret_cast<char*>(pixels.data()), pixels.size()*sizeof(int32_t));
	ofile.close();
	
	auto after_writing = myclock();
	
	std::cout << "Writing took " << to_millis(after_writing - after_processing) << "ms " << std::endl;
	
	return 0;
}

