#include "timer.h"

int timer_set_frequency(unsigned char timer, unsigned long freq) {

	if (timer < 0 || timer > 2) {
		printf("Timer %u does not exist ...\n", timer);
		return 1;
	}

	if (freq < FREQ_MIN) {
		printf("Frequency below minimum frequency accepted!\n");
		return 1;
	}

	/* getting timer's 4 lsb digits to create control word */
	unsigned char config;
	timer_get_conf(timer, &config);
	unsigned char timerLSB = config & (BIT(0) | BIT(1) | BIT(2) | BIT(3));

	/* initial value to be inserted to timer */
	unsigned long initialCountingValue = TIMER_FREQ / freq;
	// getting both lsb and msb from the initial value
	unsigned long lsb = (initialCountingValue & 0x00FF);
	unsigned long msb = ((initialCountingValue & 0xFF00) >> 8);

	/* writing the control word to the control register */
	unsigned long controlWord = TIMER_LSB_MSB | timerLSB;
	unsigned long timer_addr, timer_sel;
	switch (timer) {
	case 0:
		timer_sel = TIMER_SEL0;
		timer_addr = TIMER_0;
		break;
	case 1:
		timer_sel = TIMER_SEL1;
		timer_addr = TIMER_1;
		break;
	case 2:
		timer_sel = TIMER_SEL2;
		timer_addr = TIMER_2;
		break;
	default:
		break;
	}
	controlWord = controlWord | timer_sel;

	if (sys_outb(TIMER_CTRL, controlWord) != OK) {
		printf("Error writing to the Control Register!\n");
		return 1;
	}

	if (sys_outb(timer_addr, lsb) != OK) {
		printf("Error writing LSB to the timer!\n");
		return 1;
	}
	if (sys_outb(timer_addr, msb) != OK) {
		printf("Error writing MSB to the timer!\n");
		return 1;
	}

	return 0;
}

int timer_subscribe_int(int* g_hookid_timer) {

	/* setting interruption request policy */
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, g_hookid_timer) != OK) {
		printf("Error setting IRQ policy on Timer 0 ...\n");
		return -1;
	}

	/* enabling the DD to send the EOI command to the PIC */
	if (sys_irqenable(g_hookid_timer) != OK) {
		printf("Error enabling IRQ ...\n");
		return -1;
	}

	/* returns value of initial g_hook_id, in this case, 0
	 and a negative value, like -1, on failure */
	return 0;
}

int timer_unsubscribe_int(int* g_hookid_timer) {

	/* disabling the DD to send the EOI command to the PIC */
	if (sys_irqdisable(g_hookid_timer) != OK) {
		printf("Error disabling IRQ ...\n");
		return 1;
	}

	/* removing interruption request policy */
	if (sys_irqrmpolicy(g_hookid_timer) != OK) {
		printf("Error removing IRQ policy on Timer 0 ...\n");
		return 1;
	}

	return 0;
}

int timer_get_conf(unsigned char timer, unsigned char *st) {

	// calculating the address of the selected timer
	unsigned long timer_addr = timer + TIMER_0;
	unsigned long controlWord = TIMER_RB_CMD | TIMER_RB_COUNT_
			| TIMER_RB_SEL(timer);

	// writing to the control register
	if (sys_outb(TIMER_CTRL, controlWord) != OK) {
		printf("Error writing to the Control Register!\n");
		return 1;
	}

	// converting st data type from char* to ulong*
	unsigned long *st_long_pointer = (unsigned long *) st;

	// retrieving the configuration from the timer
	if (sys_inb(timer_addr, st_long_pointer) != OK) {
		printf("Error reading timer's %u configuration!\n", timer);
		return 1;
	}

	// reversing the previous data type conversion
	st = (unsigned char *) st_long_pointer;

	return 0;
}
