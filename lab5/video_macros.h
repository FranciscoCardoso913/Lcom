#ifndef VIDEO_MACROS
#define VIDEO_MACROS

#include <lcom/lcf.h>

#define INDEXED_MODE 0x105
#define AH_REG 0x4F
#define AL_SET_VBE_MODE 0x02
#define AL_RETURN_VBE_MODE 0x01
#define AL_RETURN_VBE_CTRL 0x00
#define LINEAR_BUFFER BIT(14)
#define INT_INTERFACE 0x10
#define FIRST_BYTE_MASK 0x000000FF
#define SECOND_BYTE_MASK 0x0000FF00
#define THIRD_BYTE_MASK 0x00FF0000
#define MASK_N_BITS(n) (BIT(n) - 1)
#define MASK_32_BITS 0xffffffff

#endif
