#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/com.h>
#include "i8254.h"

#ifndef __TIMER_H
#define __TIMER_H

/** @defgroup Timer
 * @{
 *
 * Functions for using the i8254 timers
 */

/**
 * @brief Changes the operating frequency of a timer
 * 
 * Must not change the 3 LSBs (mode and BCD/binary) of the timer's control word.
 * 
 * @param timer Timer to configure. (Ranges from 0 to 2)
 * @param freq Timer operating frequency
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_set_frequency(unsigned char timer, unsigned long freq);

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @param Timer's 0 initial hook id
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int timer_subscribe_int(int* g_hookid_timer);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @param Timer's 0 kernel-changed hook id
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int(int* g_hookid_timer);

/**
 * @brief Reads the input timer configuration via read-back command
 *
 * @param timer Timer whose config to read (Ranges from 0 to 2)
 * @param st    Address of memory position to be filled with the timer config
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_get_conf(unsigned char timer, unsigned char *st);

/**@}*/

#endif /* __TIMER_H */
