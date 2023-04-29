#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lab3.h>

/* IRQ to (un)subscribe keyboard */
#define KBD_IRQ 1
/* Check if Is BREAK or MAKE code */
#define BR_MK BIT(7)
/* The Escape Scancode */
#define ESC_BRK 0x81
/* Two bytes scancode */
#define TWO_BYTE 0xE0

/* Ports to Read/Write */
#define STATUS_REG 0x64
#define IN_BUF 0x64
#define IN_BUF_ARG 0x60
#define OUT_BUF 0x60

/* Command PC-AT/PS2 at port 0x64*/
#define READ_CMD_B 0x20         /* Returns CMD B at OUT_BUF */
#define WRITE_CMD_B 0x60        /* takes a CMD B at port 0x60 */
#define CHECK_KBC 0xAA          /* Returns 0x55/0xFC(OK/ERROR) */
#define CHECK_KBD_INT 0xAB      /* Returns 0 if OK */
#define DISABLE_KBD_I 0xAD
#define ENABLE_KBD_I 0xAE

/* KBC CONTROL BITS */
#define PARITY_ERR BIT(7)
#define TIMEOUT_ERR BIT(6)
#define AUX BIT(5)
#define INH BIT(4)
#define A2 BIT(3)
#define SYS BIT(2)
#define IBF BIT(1)
#define OBF BIT(0)

/* KBC "Command Byte" */
#define DIS2 BIT(5)
#define DIS BIT(4)
#define INT2 BIT(1)
#define INT BIT(0)

#endif
