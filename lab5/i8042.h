#ifndef _LCOM_I8042_H
#define _LCOM_I8042_H

#include <lcom/lcf.h>

#define KBC_IRQ 1

/* Constant Make/Break codes */
#define ESC_CODE 0x81
#define TWO_BYTE_CODE 0xE0

/* Kbc registers */

#define OUT_BUF 0x60
#define ARG_REG 0x60
#define STAT_REG 0x64
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

/* Delay in ticks */
#define DELAY 20

#endif // _LCOM_I8042_H
