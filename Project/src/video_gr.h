#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

/**
 * @file video_gr.h
 */

/**
 *	@defgroup vg Video_Graphics
 *	@{
 *
 *	Functions for using the graphics card
 */

#define BIT(n) (0x01<<(n))

/* Screen's resolution */
#define H_RES             	800		/**< Screen's resolution's width */
#define V_RES		  		600		/**< Screen's resolution's height */

/* brief Colors */
#define BLACK				0x000000
#define RED					0xff0000
#define GREEN				0x00ff00
#define BLUE				0x0000ff
#define WHITE				0xffffff
#define BG_COLOR			0xff00ff
#define BORDER_COLOR		0x774503
#define SNAKE_COLOR			0x005109
#define GRASS_COLOR			0x00aa2d
#define MOUSE_BG_COLOR		0xd87e09
#define LETTER_BORDER_COLOR	0x8e0b0b

/**
 * @brief Initializes the video module in graphics mode
 *
 * 	Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen,
 *  and the number of colors
 *
 * @param mode 24-bit color indexed mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

/**
* @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
*
* @return 0 upon success, non-zero upon failure
*/
int vg_exit(void);

/**
 * 	@brief Sets a pixel's color
 *
 * 	Sets pixel's color on coordinates (x,y).
 *
 * 	@param x Pixel's x coordinate on the screen
 * 	@param y Pixel's y coordinate on the screen
 * 	@param color RGB color to set
 */
void draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * 	@brief Draws a rectangle on the screen
 *
 * 	Draws a rectangle on the screen based on the (x,y) coordinates of
 * 	its left-upper corner, width, height and color.
 *
 * 	@param x Rectangle's left-upper corner x coordinate
 * 	@param y Rectangle's left-upper corner y coordinate
 * 	@param width Rectangle's width
 * 	@param height Rectangle's height
 * 	@param color RGB color to set
 */
void vg_drawRect(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height, unsigned int color);

/**
 *  @brief Prints game borders
 *
 *  Prints game borders on the screen using function vg_drawRect().
 */
void vg_print_borders();

/**
 * 	@brief Draws a png image on the screen
 *
 * 	Draws a png image on the screen based on the (x,y) coordinates of
 * 	its left-upper corner, width and height.
 *
 * 	@param image PNG image to be printed on the screen
 * 	@param width Image's width
 * 	@param height Image's height
 * 	@param start_x Image's left-upper corner x coordinate
 * 	@param start_y Image's left-upper corner y coordinate
 */
void vg_png(unsigned char* image, int width, int height, uint16_t start_x, uint16_t start_y);

/**
 * 	@brief Draws a tile from the "font.png" image on the screen
 *
 * 	Draws a letter tile on the screen, based on the required letter,
 *	the width and height of the "font.png" image, and the (x,y) coordinates
 *	where to print it.
 *
 * 	@param image PNG image to print tiles from ("font.png")
 * 	@param width Image's width
 * 	@param height Image's height
 * 	@param start_x Tile's left-upper corner x coordinate
 * 	@param start_y Tile's left-upper corner y coordinate
 */
void vg_tile(unsigned char* image, char letter, int width, int height, uint16_t start_x, uint16_t start_y);

/**
 * 	@brief Clears snake's left part of the playable screen
 *
 * 	Clears the snake's part of the screen setting its background color to BLACK.
 */
void vg_snake_clear();

/**
 * 	@brief Clears cursor's right part of the playable screen
 *
 * 	Clears the cursor's part of the screen setting its background color to BLACK.
 */
void vg_cursor_clear();

/**
 * 	@brief Clears the entire screen
 *
 * 	Clears the screen setting its background color to BLACK.
 */
void vg_clear();

/**
 * 	@brief Copies double_buffer memory to video_mem
 */
void vg_copy();

/**
 * 	@brief Deallocates double_buffer memory
 */
void vg_free();

/**@}*/

#endif /* __VIDEO_GR_H */
