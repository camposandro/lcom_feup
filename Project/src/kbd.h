#include "i8042.h"

#ifndef __KBD_H
#define __KBD_H

/** @defgroup Keyboard
 * @{
 *
 * Functions for using the i8042 keyboard controller
 */

/**
 *	@brief Subscribes and enables Keyboard interrupts
 *	@param g_hookid_kbd Keyboard's initial hook id - 1
 *	@return Returns 1 on success, -1 otherwise
 */
int kbd_subscribe_int(int* g_hookid_kbd);

/**
 *	@brief Unsubscribes and disables Keyboard interrupts
 *	@param g_hookid_kbd Keyboard's kernel-changed hook id
 *	@return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int(int* g_hookid_kbd);

/**
 *	@brief Reads keyboard Status Register at port 0x64
 *	@param status Status retrieved from the status register
 *	@return Returns status from the register
 */
int readStatusRegister(unsigned long* status);

/**
 *	@brief Reads Output Buffer - written in C
 *	@param scancode Scancode retrieved from the Output Buffer
 *	@return Returns 0 in sucess and 1 otherwise, and
 *	also returns the scancode read from the Output Buffer
 *	as an argument of it
 */
int readOutBuffer(unsigned long* scancode);

/**
 *  @brief Keyboard's assembly handler
 *
 *  Assembly sub-routine attempting to read the scancode from the
 *  output buffer. Verifies if the data is valid or not and, in case
 *  of success, loads the scancode into the "scancode" variable.
 *
 *  @return Returns 0 on scancode reading failure, non 0 otherwise.
 */
extern int kbd_asm_handler();

/**@}*/

#endif /* __KBD_H */
