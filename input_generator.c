#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// change size to make image bigger/smaller
#define IMGSIZE 0x3000  // 0x6000

#define image(x,y) pixels[((unsigned int)y*size+x)]

// generates an image
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s [create_file]", argv[0]);
        return 0;
    }

	unsigned long long int size;
	unsigned long long int x, y;

	FILE *out;
	srand(time(NULL));

	out = fopen(argv[1], "wb");
	if (out == NULL ) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	size = IMGSIZE;
	printf("%lld\n", size);

	fwrite(&size, sizeof(size), 1, out);
	fflush(out);

	int *pixels = (int*) malloc(size * size * sizeof(int));
	if (pixels == NULL ) {
		perror("malloc pixels");
		exit(EXIT_FAILURE);
	}

	for (y = 0; y < size; y++) {
		for (x = 0; x < size; x++) {
			image(x,y)= (rand()&0xFFFF);
		}
	}
	fwrite(pixels, size * size * sizeof(int), 1, out);

	fflush(out);
	fclose(out);
	free(pixels);

	return EXIT_SUCCESS;
}
