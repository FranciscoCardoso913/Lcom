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

// delay for polling
#define DELAY_US 20000

// interrupt line for keyboard
#define KBD_IRQ 1

// useful ports
#define ESC_BREAK 0x81
#define STAT_REG 0x64
#define OUT_BUF 0x60

// errors: parity and timeout
#define PAR_ERR BIT(7)
#define TO_ERR BIT(6)

// make or break bit
#define MAKE_BIT BIT(7)

// if two byte code then first byte is 0xE0
#define TWO_BYTE_CODE 0xE0

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
void (kbc_ih)();


#endif /* _LCOM_KEYBOARD_H */
