#include <minix/syslib.h>
#include <minix/driver.h>
#include <minix/drivers.h>
#include "kbd.h"
#include "mouse.h"

int mouse_subscribe_int(int* g_hookid_mouse) {

	// setting KBC interruption request policy
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, g_hookid_mouse) != OK) {
		printf("Error setting IRQ policy on KBC interrupts ...\n");
		return -1;
	}

	// enabling the DD to send the EOI command to the PIC
	if (sys_irqenable(g_hookid_mouse) != OK) {
		printf("Error enabling IRQ ...\n");
		return -1;
	}

	return 1;
}

int mouse_unsubscribe_int(int* g_hookid_mouse) {

	// disabling the DD to send the EOI command to the PIC
	if (sys_irqdisable(g_hookid_mouse) != OK) {
		printf("Error disabling IRQ ...\n");
		return 1;
	}

	// removing interruption request policy
	if (sys_irqrmpolicy(g_hookid_mouse) != OK) {
		printf("Error removing IRQ policy on KBC interrupts ...\n");
		return 1;
	}

	return 0;
}

int write_cmd(unsigned long port, unsigned long cmd) {

	unsigned long status;
	/* executed while the command is not written */
	while (1) {
		/* reads status from Status Register */
		readStatusRegister(&status);
		/* if Input Buffer is not full*/
		if ((status & IBF) == 0)
		{
			/* write command */
			if (sys_outb(port, cmd) != OK) {
				printf("Error writing the command %x to port 0x%x!\n", cmd, port);
				return 1;
			}
			break;
		}

		tickdelay(micros_to_ticks(WAIT_KBC));
	}

	return 0;
}

int mouse_write_cmd(unsigned long cmd) {

	unsigned long response;
	/* executed while the command is not written */
	while (1) {
		/* write command 0xD4 to STATUS_REG */
		if (!write_cmd(STAT_REG, MOUSE_CMD)) {
			/* write command to IN_BUF */
			if (!write_cmd(IN_BUF, cmd)) {
				/* wait for device's response */
				tickdelay(micros_to_ticks(WAIT_KBC));
				sys_inb(OUT_BUF, &response);
				/* if it is a non-error response, no need to
				 * resend the command */
				if (response != NACK || response != ERROR) {
					break;
				}
			}
		}
		tickdelay(micros_to_ticks(WAIT_KBC));
	}

	return 0;
}
