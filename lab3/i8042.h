#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KEYBOARD_IRQ 1

#define ESC_BREAK 0x81
#define TWO_BYTES 0xe0

#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_OUT_BUF 0x60
#define KBC_IN_BUF_UP 0x64
#define KBC_IN_BUF_DN 0x60

#define KBC_READ_CMD 0x20
#define KBC_WRITE_CMD 0x60

#define KBC_OUT_FULL 0x01
#define KBC_IN_FULL 0x02

#define KBC_PERROR BIT(7)
#define KBC_TERROR BIT(6)
#define KBC_ERROR (BIT(7) | BIT(6))

#define WAIT_KBC 2000

#define MAKE_OR_BREAK BIT(7)
#define MOUSE_DATA BIT(5)

#endif /* _LCOM_I8042_H_ */
