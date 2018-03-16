#include <minix/syslib.h>
#include <minix/driver.h>
#include <minix/drivers.h>
#include "rtc.h"

int rtc_time(unsigned long *sec, unsigned long *min, unsigned long *hour) {

	sys_outb(RTC_ADDR_REG, RTC_SEC);
	sys_inb(RTC_DATA_REG, sec);

	sys_outb(RTC_ADDR_REG, RTC_MIN);
	sys_inb(RTC_DATA_REG, min);
	sys_outb(RTC_ADDR_REG, RTC_HOUR);
	sys_inb(RTC_DATA_REG, hour);

	return 0;
}

int rtc_date(unsigned long *day, unsigned long *month, unsigned long *year) {

	sys_outb(RTC_ADDR_REG, RTC_DAY);
	sys_inb(RTC_DATA_REG, day);
	sys_outb(RTC_ADDR_REG, RTC_MONTH);
	sys_inb(RTC_DATA_REG, month);

	sys_outb(RTC_ADDR_REG, RTC_YEAR);
	sys_inb(RTC_DATA_REG, year);

	return 0;
}
