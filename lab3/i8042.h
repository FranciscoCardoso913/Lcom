#ifndef _LCOM_I8042_H
#define _LCOM_I8042_H

#include <lcom/lcf.h>

#define KBD_IRQ 1
#define ESC_CODE 0x81
#define TWO_BYTE_CODE 0xE0
#define OUT_BUF 0x60
#define STAT_REG 0x64
#define IN_BUF 0x64 
#define OUT_BUF_FULL BIT(0)
#define KBC_PAR_ERROR BIT(7)
#define KBC_TO_ERROR BIT(6)

#endif // _LCOM_I8042_H