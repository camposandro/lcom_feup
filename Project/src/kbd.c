#include <minix/syslib.h>
#include <minix/driver.h>
#include <minix/drivers.h>
#include "kbd.h"

int kbd_subscribe_int(int* g_hookid_kbd) {

	// setting KBC interruption request policy
	if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, g_hookid_kbd) != OK) {
		printf("Error setting IRQ policy on KBC interrupts ...\n");
		return -1;
	}

	// enabling the DD to send the EOI command to the PIC
	if (sys_irqenable(g_hookid_kbd) != OK) {
		printf("Error enabling IRQ ...\n");
		return -1;
	}

	/* returns value of initial g_hook_id, in this case, 1
	and a negative value, like -1, on failure */
	return 1;
}

int kbd_unsubscribe_int(int* g_hookid_kbd) {

	// disabling the DD to send the EOI command to the PIC
	if (sys_irqdisable(g_hookid_kbd) != OK) {
		printf("Error disabling IRQ ...\n");
		return 1;
	}

	// removing interruption request policy
	if (sys_irqrmpolicy(g_hookid_kbd) != OK) {
		printf("Error removing IRQ policy on KBC interrupts ...\n");
		return 1;
	}

	return 0;
}

int readStatusRegister(unsigned long* status) {

	if (sys_inb(STAT_REG, status) != OK) {
		printf("Error reading the Status Register!\n");
		return 1;
	}

	return 0;
}

int readOutBuffer(unsigned long* scancode) {

	if (sys_inb(OUT_BUF, scancode) != OK) {
		printf ("Error reading data from OUT_BUF!\n");
		return 1;
	}

	return 0;
}
