#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <time.h>
#include <stdint.h>

#define pixel(x,y) pixels[((y)*size)+(x)]

//#define DEBUG

void print(int *pixels, int size) {
	int x, y;
	for (y = 0; y < size; y++) {
		for (x = 0; x < size; x++) {
			printf("%10d ", pixel(x,y));
		}
		printf("\n");
		fflush(stdout);
	}
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: %s [input_file] [output_file]\n", argv[0]);
        return 0;
    }

	FILE *in;
	FILE *out;

    clock_t start;
    clock_t end;

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	out = fopen(argv[2], "wb");
	if (out == NULL) {
		perror(argv[2]);
		exit(EXIT_FAILURE);
	}

	long long int s, size, mid;
	int x, y;
	long long int a, d;
	double SQRT_2;

	fread(&size, sizeof(size), 1, in);

	fwrite(&size, sizeof(size), 1, out);

	int *pixels = (int *) malloc(size * size * sizeof(int));

    start = clock();
	if (!fread(pixels, size * size * sizeof(int), 1, in)) {
		perror("read error");
		exit(EXIT_FAILURE);
	}
    end = clock();
    printf("time to read: %ju (%f s)\n", (uintmax_t)(end - start), (float)(end - start) / CLOCKS_PER_SEC);

#ifdef DEBUG
	printf("origin: %lld\n", size);
	print(pixels, size);
#endif

	// haar
    start = clock();

	SQRT_2 = sqrt(2);
	for (s = size; s > 1; s /= 2) {
		mid = s / 2;
		// row-transformation
		for (y = 0; y < mid; y++) {
			for (x = 0; x < mid; x++) {
				a = pixel(x,y);
				a = (a+pixel(mid+x,y))/SQRT_2;
				d = pixel(x,y);
				d = (d-pixel(mid+x,y))/SQRT_2;
				pixel(x,y) = a;
				pixel(mid+x,y) = d;
			}
		}

#ifdef DEBUG
		printf("after row-transformation: %lld\n", mid);
		print(pixels, size);
#endif
		// column-transformation
		for (y = 0; y < mid; y++) {
			for (x = 0; x < mid; x++) {
				a = pixel(x,y);
				a = (a+pixel(x,mid+y))/SQRT_2;
				d = pixel(x,y);
				d = (d-pixel(x,mid+y))/SQRT_2;
				pixel(x,y) = a;
				pixel(x,mid+y) = d;
			}
		}

#ifdef DEBUG
		printf("after column-transformation: %lld\n", mid);
		print(pixels, size);
#endif
	}

    end = clock();
    printf("time to haar: %ju (%f s)\n", (uintmax_t)(end - start), (float)(end - start) / CLOCKS_PER_SEC);

    start = clock();

	fwrite(pixels, size * size * sizeof(int), 1, out);

    end = clock();
    printf("time to write: %ju (%f s)\n", (uintmax_t)(end - start), (float)(end - start) / CLOCKS_PER_SEC);

	free(pixels);

	fclose(out);
	fclose(in);

	return EXIT_SUCCESS;
}
