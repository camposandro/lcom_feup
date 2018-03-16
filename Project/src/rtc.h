#ifndef __RTC_H_
#define __RTC_H_

/** @defgroup RTC
 * @{
 *
 * Functions for using the Real Time Clock
 */

/* RTC registers */
#define RTC_ADDR_REG 0x70	/**< RTC address register */
#define RTC_DATA_REG 0x71	/**< RTC data register */

/* RTC modes */
#define RTC_SEC 0x00	/**< RTC seconds register */
#define RTC_MIN 0x02	/**< RTC minutes register */
#define RTC_HOUR 0x04	/**< RTC hour register */
#define RTC_DAY 0x07	/**< RTC day register */
#define RTC_MONTH 0x08	/**< RTC month register */
#define RTC_YEAR 0x09	/**< RTC year register */

/**
 *	@brief Reads the computer's current time
 *
 *	Reads the time and changes the value pointed by its arguments
 *	to the seconds, minutes and hour of the day, respectively.
 *
 * 	@param sec Current seconds
 * 	@param min Current minute
 * 	@param hour Current hour
 * 	@return Returns 0 upon success and non-zero otherwise
 */
int rtc_time(unsigned long *sec, unsigned long *min, unsigned long *hour);

/**
 *	@brief Reads the computer's current date
 *
 *	Reads the date and changes the value pointed by its arguments
 *	to the day, month and year, respectively.
 *
 * 	@param day Current day
 * 	@param month Current month
 *	@param year Current year
 * 	@return Returns 0 upon success and non-zero otherwise
 */
int rtc_date(unsigned long *day, unsigned long *month, unsigned long *year);

/**@}*/

#endif /* __RTC_H_ */
