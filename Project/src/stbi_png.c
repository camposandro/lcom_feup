#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stbi_png.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char* stbi_png_load(int* width, int* height, const char* image_path) {

	int n;
	unsigned char* image = stbi_load(image_path, width, height, &n, 3);
	if(image == NULL) {
		printf("Couldn't open PNG image, verify if image path is valid!\n");
		return NULL;
	}

	return image;
}

void stbi_free(unsigned char* image) {

	stbi_image_free(image);
}
