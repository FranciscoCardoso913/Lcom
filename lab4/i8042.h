#ifndef _LCOM_I8042_H
#define _LCOM_I8042_H

#include <lcom/lcf.h>

#define MOUSE_IRQ 12
#define KBC_IRQ 1

/* Constant Make/Break codes */
#define ESC_CODE 0x81
#define TWO_BYTE_CODE 0xE0

/* Kbc registers */

#define OUT_BUF 0x60
#define ARG_REG 0x60
#define STATUS_REG 0x64
#define IN_BUF 0x64

/* Status bits */
#define KBC_ST_IBF BIT(1)
#define KBC_ST_OBF BIT(0)
#define KBC_ST_AUX BIT(5)
#define KBC_PAR_ERROR BIT(7)
#define KBC_TO_ERROR BIT(6)

/* Kbc commands */
#define READ_CMD 0x20
#define WRITE_CMD 0x60
#define CHECK_KBC_CMD 0xAA
#define CHECK_KBD_INTERFACE 0xAB
#define DISABLE_KBD 0xAD
#define ENABLE_KBD 0xAE

/* Change command byte */
#define ENABLE_KBC_INTERRUPT 0x01
#define DISABLE_KBC_INTERFACE BIT(4)

/* Check KBD interface returns */
#define KBD_INTERFACE_FAILURE 0xFC
#define KBD_INTERFACE_SUCCESS 0x55

/* Mouse commands */
#define WRITE_BYTE_MOUSE 0xD4
#define ENABLE_DATA_REP 0xF4
#define DISABLE_DATA_REP 0xF5

/* Mouse byte info */
#define MOUSE_LB BIT(0)
#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)
#define MOUSE_ALWAYS_ON BIT(3)
#define MOUSE_MSBX_DELTA BIT(4)
#define MOUSE_MSBY_DELTA BIT(5)
#define MOUSE_X_OVFL BIT(6)
#define MOUSE_Y_OVFL BIT(7)

/* Mouse commands responses */
#define MOUSE_ACK 0xFA
#define MOUSE_NACK 0xFE
#define MOUSE_ERROR 0xFC

/* Delay in ticks */
#define DELAY 20

/* General errors */
#define TIMEOUT_ERR 2

#endif //_LCOM_I8042_H
