#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <omp.h>

typedef std::chrono::steady_clock::time_point chrono_time;
inline auto chrono_count(const auto& difference) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(difference).count();
}
inline auto chrono_now() {
	return std::chrono::steady_clock::now();
}

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
    
    chrono_time begin = chrono_now();

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
	
	chrono_time after_reading = chrono_now();
	std::cout << "Reading took " << chrono_count(after_reading - begin) << "ms " << std::endl;
	std::cout << "Processing an image of " << size << "x" << size << " dimensions with up to " << omp_get_max_threads()  << " threads" << std::endl;
	
	process(pixels, size);
	chrono_time after_processing = chrono_now();
	
	std::cout << "Processing took " << chrono_count(after_processing - after_reading) << "ms " << std::endl;
	
	ofile.write(reinterpret_cast<char*>(pixels.data()), pixels.size()*sizeof(int32_t));
	ofile.close();
	
	std::cout << "Writing took " << chrono_count(chrono_now() - after_processing) << "ms " << std::endl;
	
	return 0;
}

