#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lab4.h>

/* IRQ to (un)subscribe keyboard */
#define KBD_IRQ 1
/* IRQ to (un)subscribe mouse*/
#define MOUSE_IRQ 12

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
#define DISABLE_MOUSE_I 0xA7
#define ENABLE_MOUSE_I 0xA8
#define CHECK_MOUSE_I 0xA9      /* Returns 0 if OK */
#define WRITE_BYTE_MOUSE 0xD4   /* takes a BYTE at port 0x60 */

/* KBD CONTROL BITS */
#define PARITY_ERR BIT(7)
#define TIMEOUT_ERR BIT(6)
#define AUX BIT(5)
#define INH BIT(4)
#define A2 BIT(3)
#define SYS BIT(2)
#define IBF BIT(1)
#define OBF BIT(0)

/* KBD "Command Byte" */
#define DIS2 BIT(5)
#define DIS BIT(4)
#define INT2 BIT(1)
#define INT BIT(0)

/* MOUSE COMMANDS */
#define RESET 0xFF
#define RESEND 0xFE
#define SET_DEFAULTS 0xF6
#define DISABLE_DATA_REP 0xF5
#define ENABLE_DATA_REP 0xF4
#define SET_SAMPLE_RATE 0xF3
#define SET_REMOTE_MODE 0xF0
#define READ_DATA 0xEB
#define SET_STREAM_MODE 0xEA
#define STATUS_REQUEST 0xE9
#define SET_RESOLUTION 0xE8
#define SET_SCALING_2_1 0xE7
#define SET_SCALING_1_1 0xE6

/* RESPONSES TO 0XD4 */
#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

/* MOUSE STATUS */
#define REMOTE BIT(6)
#define DISABLE BIT(5)
#define SCALING BIT(4)
#define RESOLUTION BIT(2)
#define SAMPLE BIT(0)

/* MOUSE STATUS REQUEST */
#define MOUSE_ID 0x00
#define MOUSE_TYPE 0x03

/* MOUSE PACKET BYTE */
#define LB BIT(0)
#define RB BIT(1)
#define MB BIT(2)
#define X_MSB BIT(4)
#define Y_MSB BIT(5)
#define X_OV BIT(6)
#define Y_OV BIT(7)

#endif
