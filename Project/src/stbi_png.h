#ifndef __STBI_PNG_H
#define __STBI_PNG_H

/**
 *	@file stbi_png.h
 *	@brief Credits to: https://github.com/nothings
 *	Based on: https://github.com/nothings/stb/blob/master/stb_image.h
 */

/**
 *	@brief Loads PNG image, returning it
 *	@param width Loaded image's width
 *	@param height Loaded image's height
 *	@param image_path PNG image path
 */
unsigned char* stbi_png_load(int* width, int* height, const char* image_path);

/**
 *	@brief Frees image loaded with stbi_png_load
 *	@param image Image to be freed
 */
void stbi_free(unsigned char* image);

#endif /* __STBI_PNG_H */
