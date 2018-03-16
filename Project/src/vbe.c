#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include "vbe.h"
#include "lmlib.h"
#include "video_gr.h"

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t* vmi_p) {

	/* variables declaration */
	struct reg86u r;
	mmap_t video_buf;
	void* virtual_mem = NULL;
	void* mem_block = NULL;

	/* mapping the physical video memory */
	virtual_mem = lm_init();
	if (virtual_mem == NULL) {
		printf("Error calling lm_init()!\n");
		return 1;
	}

	/* allocating memory block in video buf */
	mem_block = lm_alloc(sizeof(vbe_mode_info_t), &video_buf);
	if (mem_block == NULL) {
		printf("Error calling lm_alloc()!\n");
		return 1;
	}

	r.u.w.ax = 0x4F01;					/* VBE get mode info */
	/*translate the buffer linear address to a far pointer */
	r.u.w.es = PB2BASE(video_buf.phys);	/* set a segment base */
	r.u.w.di = PB2OFF(video_buf.phys); 	/* set the offset accordingly */
	r.u.w.cx = mode | BIT(14);			/* set bit 14 to use linear frame buffer model */
	r.u.b.intno = 0x10;

	if (sys_int86(&r) != OK) { /* call BIOS */
		printf("Error calling vbe_get_mode_info(): sys_int86() failed!\n");
		/* free video memory allocated */
		lm_free(&video_buf);
		return 1;
	}

	*vmi_p = *((vbe_mode_info_t*) video_buf.virtual);
	lm_free(&video_buf);
	return 0;
}
