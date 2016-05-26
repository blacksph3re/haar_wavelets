#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <omp.h>

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
auto to_millis(auto input) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(input).count();
}

auto myclock() {
	return std::chrono::system_clock::now();
}
#endif

void process(std::vector<int32_t>& pixels, int32_t size) {
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
	std::ofstream ofile;
	
	if (argc != 3) {
        std::cout << "usage: " << argv[0] << " [input_file] [output_file]" << std::endl;
        return 255;
    }

    auto begin = myclock();

    // Open the files
    ifile.open(argv[1], std::ios::binary | std::ios::in);
	ofile.open(argv[2], std::ios::binary | std::ios::out | std::ios::trunc);
	if(!ifile.is_open() || !ofile.is_open()) {
		std::cout << "couldn't open file" << std::endl;
		return -1;
	}
	
	// Read the data into a vec<vec<int>>
	int32_t size;
	ifile.read(reinterpret_cast<char*>(&size), sizeof(size));
	ofile.write(reinterpret_cast<char*>(&size), sizeof(size));

	
	std::vector<int32_t> pixels;
	pixels.resize(size*size, 0);
	ifile.read(reinterpret_cast<char*>(pixels.data()), size*size*sizeof(int32_t));
	ifile.close();
	
	auto after_reading = myclock();
	std::cout << "Reading took " << to_millis(after_reading - begin) << "ms " << std::endl;
	std::cout << "Processing an image of " << size << "x" << size << " dimensions with up to " << omp_get_max_threads()  << " threads" << std::endl;
	
	process(pixels, size);
	auto after_processing = myclock();
	
	std::cout << "Processing took " << to_millis(after_processing - after_reading) << "ms " << std::endl;
	
	ofile.write(reinterpret_cast<char*>(pixels.data()), pixels.size()*sizeof(int32_t));
	ofile.close();
	
	auto after_writing = myclock();
	
	std::cout << "Writing took " << to_millis(after_writing - after_processing) << "ms " << std::endl;
	
	return 0;
}

