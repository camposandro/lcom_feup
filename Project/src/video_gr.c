#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "video_gr.h"
#include "lmlib.h"
#include "vbe.h"

static phys_bytes video_phys;	/*< VRAM's physical address */
static char* video_mem;			/*< VRAM's virtual address */
static char* double_buffer;		/*< DOUBLE-BUFFER's virtual address */

static uint16_t h_res;			/**< Screen's horizontal resolution in pixels */
static uint16_t v_res;			/**< Screen's vertical resolution in pixels */
static uint8_t bits_per_pixel; 	/**< Number of bits per pixel */

void* vg_init(unsigned short mode) {

	int r;
	struct reg86u reg86;
	struct mem_range mr;
	int vram_size;
	vbe_mode_info_t info;

	/* Gets vbe mode's information */
	if (vbe_get_mode_info(mode, &info) != 0) return NULL;

	h_res = info.XResolution;
	v_res = info.YResolution;
	bits_per_pixel = info.BitsPerPixel;
	video_phys = info.PhysBasePtr;

	/* Sets vbe's mode */
	reg86.u.w.ax = 0x4F02;
	reg86.u.w.bx = BIT(14) | mode;
	reg86.u.b.intno = 0x10;
	if(sys_int86(&reg86) != OK) {
		printf("vg_init: sys_int86() failed\n");
		return NULL;
	}

	vram_size = h_res * v_res * (bits_per_pixel / 8);
	/* Allow memory mapping */
	mr.mr_base = video_phys;
	mr.mr_limit = mr.mr_base + vram_size;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Maps video memory */
	video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);

	if (video_mem == MAP_FAILED)
		panic("vg_init: couldn't map video memory\n");

	/* Allocates double buffer */
	double_buffer = (char*) malloc(vram_size);

	return video_mem;
}

int vg_exit() {

	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.b.ah = 0x00;    /* Set Video Mode function */
	reg86.u.b.al = 0x03;    /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	}

	return 0;
}

/** Sets a pixel's color on coordinates (x,y) */
void draw_pixel(uint16_t x, uint16_t y, uint32_t color) {

	char* vram = double_buffer;

	/* if pixel is out of range or color is the same as backgrounds' */
	if (x >= h_res || y >= v_res || color == BG_COLOR) return;

	/* calculating pixel's position */
	vram += (y * h_res + x) * (bits_per_pixel / 8);

	/* transforming in RGB */
	*vram = color & BLUE;
	*(vram + 1) = (color & GREEN) >> 8;
	*(vram + 2) = (color & RED) >> 16;
}

/* draws rectangle */
void vg_drawRect(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height, unsigned int color) {

	size_t i, j;
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			draw_pixel(start_x + i, start_y + j, color);
		}
	}
}

/* draws game border */
void vg_print_borders() {

	vg_drawRect(0,0,800,5, BORDER_COLOR);
	vg_drawRect(0,0,5,600, BORDER_COLOR);
	vg_drawRect(795,0,5,600, BORDER_COLOR);
	vg_drawRect(0,595,800,5, BORDER_COLOR);
	vg_drawRect(495,0,5,600, BORDER_COLOR);
}

/** Draws a png image, with left corner (x,y) */
void vg_png(unsigned char* image, int width, int height, uint16_t start_x, uint16_t start_y) {

	size_t x, y;
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			uint32_t rgb[3];
			size_t c;
			for (c = 0; c < 3; c++)
				rgb[c] = image[(x + y * width) * 3 + c];
			int32_t color = ((rgb[0] << 16) & RED) | ((rgb[1] << 8) & GREEN) | (rgb[2] & BLUE);
			draw_pixel(start_x + x, start_y + y, color);
		}
	}
}

/** Draws a specified letter from a given font */
void vg_tile(unsigned char* image, char letter, int width, int height, uint16_t start_x, uint16_t start_y) {

	size_t row, col;
	size_t x, y, xi, yi;
	size_t pos = (size_t) letter;

	yi = (size_t) (pos-48) / 16;
	xi = pos - 48 - yi * 16;
	xi *= 16;
	yi *= 16;

	col = yi + 16;
	row = xi + 16;

	for (y = yi; y < col; y++) {
		for (x = xi; x < row; x++) {
			if (x == xi || x == row - 1 || y == yi || y == col - 1) {
				draw_pixel(start_x + (x - xi), start_y + (y - yi), LETTER_BORDER_COLOR);
			}
			else {
				uint32_t rgb[3];
				size_t c;
				for (c = 0; c < 3; c++)
					rgb[c] = image[(x + y * width) * 3 + c];
				int32_t color = ((rgb[0] << 16) & RED) | ((rgb[1] << 8) & GREEN) | (rgb[2] & BLUE);
				draw_pixel(start_x + (x - xi), start_y + (y - yi), color);
			}
		}
	}
}

/** Clears snake's part of the screen */
void vg_snake_clear() {

	size_t x, y;
	for (y = 5; y < 595; y++) {
		for (x = 5; x < 495; x++) {
			draw_pixel(x, y, GRASS_COLOR);
		}
	}
	vg_print_borders();
}

/** Clears cursor's part of the screen */
void vg_cursor_clear() {

	size_t x, y;
	for (y = 5; y < 595; y++) {
		for (x = 500; x < 795; x++) {
			draw_pixel(x, y, MOUSE_BG_COLOR);
		}
	}
	vg_print_borders();
}

/** Cleans double buffer, setting all pixels to black */
void vg_clear() {
	memset(double_buffer, 0, h_res * v_res * (bits_per_pixel / 8));
}

/** Copies double_buffer to video_mem */
void vg_copy() {
	memcpy(video_mem, double_buffer, h_res * v_res * (bits_per_pixel / 8));
}

/** Deallocates double buffer */
void vg_free() {
	free(double_buffer);
}
