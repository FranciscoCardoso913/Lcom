#ifndef VIDEO_MACROS
#define VIDEO_MACROS

#include <lcom/lcf.h>

#define AH_REG 0x4F
#define AL_SET_VBE_MODE 0x02
#define AL_RETURN_VBE_MODE 0x01
#define AL_RETURN_VBE_CTRL 0x00
#define LINEAR_BUFFER BIT(14)
#define INT_INTERFACE 0x10

#endif
