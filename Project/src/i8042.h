#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard.
 */

#define BIT(n) (0x01<<(n))

#define KBD_IRQ			1		/**< @brief Keyboard IRQ line */
#define MOUSE_IRQ		12		/**< @brief Mouse IRQ line */

/* KBC port addresses */
#define OUT_BUF			0x60	/**< @brief Output Buffer register */
#define IN_BUF			0x60	/**< @brief Input Buffer register */
#define STAT_REG		0x64	/**< @brief Status Register */
#define KBC_CMD_REG 	0x64	/**< @brief KBC command register */

/* KBC Status Register for I/O */
#define PAR_ERR			BIT(7)	/**< @brief Parity bit */
#define TO_ERR			BIT(6)	/**< @brief Timeout bit */
#define AUX				BIT(5)	/**< @brief AUX bit */
#define IBF				BIT(1)	/**< @brief Input buffer full bit */
#define OBF				BIT(0)	/**< @brief Output buffer full bit */

#define ESC_KEY			0x81
#define WAIT_KBC		2000

/* KBC Command Bytes */
#define READ_CMD		0x20	/**< @brief KBC read command */
#define READ_FROM		0x60	/**< @brief Register to read from */
#define WRITE_CMD		0x60	/**< @brief KBC write command */
#define WRITE_TO		0x60	/**< @brief Register to write to */

/* MOUSE related commands */
#define MOUSE_CMD		0xD4	/**< @brief Mouse command */
#define ENABLE_MOUSE	0xF4	/**< @brief Mouse enable command */
#define DISABLE_MOUSE	0xF5	/**< @brief Mouse disable command */
#define STREAM_MODE		0xEA	/**< @brief Mouse stream mode command */
#define NACK 			0xFE	/**< @brief Mouse NACK */
#define ERROR			0xFC	/**< @brief Mouse ERROR */
#define REMOTE_MODE		0xF0	/**< @brief Mouse remote mode command */
#define READ_DATA		0xEB	/**< @brief Mouse read data command */

/* MOUSE 1st packet byte */
#define LB				BIT(0)	/**< @brief Mouse left button bit */
#define RB				BIT(1)	/**< @brief Mouse right button bit */
#define MB				BIT(2)	/**< @brief Mouse middle button bit */
#define XSIGN			BIT(4)	/**< @brief Mouse delta x sign bit */
#define YSIGN			BIT(5)	/**< @brief Mouse delta y sign bit */
#define XOV				BIT(6)	/**< @brief Mouse delta x overflow bit */
#define YOV				BIT(7)	/**< @brief Mouse delta y overflow bit */

/**@}*/

#endif /* _LCOM_I8042_H */
