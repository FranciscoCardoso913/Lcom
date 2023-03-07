#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <lcom/lcf.h>

// read scancode and status to these
uint8_t scancode, status;

// size of scancode
uint8_t size;

// bytes of scancode
uint8_t bytes[2];

// hook
int hook_id;

// return value used in keyboard.c
int ret;

#define KBD_IRQ 1
#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60
#define KBD_INT 0x01
#define ESC_BREAK 0x81

#define DELAY_US 20000

#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60
#define IN_BUF 0x60

#define OBF BIT(0)
#define IBF BIT(1)
#define PAR_ERR BIT(7)
#define TO_ERR BIT(6)

#define KBC_EN BIT(0)
#define KBC_INT BIT(1)
#define MAKE_BIT BIT(7)

#define TWO_BYTE_CODE 0xE0

#define ESC_BREAK_CODE 0x81

#define KBC_READ_CMD 0x20
#define KBC_WRITE_CMD 0x60

#define KBC_DIS BIT(4)

#define RESEND 0xFE
#define ERROR 0xFC

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
void (kbc_ih)();


#endif /* _LCOM_KEYBOARD_H */
