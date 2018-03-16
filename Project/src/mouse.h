#include "i8042.h"

#ifndef __MOUSE_H
#define __MOUSE_H

/** @defgroup Mouse
 * @{
 *
 * Functions for using the i8042 keyboard controller
 */

/**
 *	@brief Subscribes and enables mouse interrupts
 *	@param g_hookid_mouse Mouse's initial hook id - 12
 *	@return Returns 1 on success, -1 otherwise
 */
int mouse_subscribe_int(int* g_hookid_mouse);

/**
 *	@brief Unsubscribes and disables mouse interrupts
 *	@param g_hookid_mouse Mouse's kernel-changed hook id
 *	@return Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe_int(int* g_hookid_mouse);

/**
 *	@brief Writes command "cmd" to port "port"
 *	@param port Port to send the command to
 *	@param cmd Command to be written to port
 *	@return Returns 0 upon success and 1 otherwise
 */
int write_cmd(unsigned long port, unsigned long cmd);

/**
 *	@brief Sends a mouse command to the KBC
 *	@param cmd Command to be written to the KBC
 *	@return Returns 0 upon success and non-zero otherwise
 */
int mouse_write_cmd(unsigned long cmd);

/**@}*/

#endif /* __MOUSE_H */
