#ifndef __VBE_H
#define __VBE_H

#include <lcom/lcf.h>
#include <lcom/lab5.h>

#define BIOS_VIDEO_SERVICES 0x10

#define VBE_MODE_INFO 0x4F01
#define VBE_SET_MODE 0x4F02
#define VBE_GET_MODE 0x4F03
#define VBE_MODE_SUPPORTED 0x4F00
#define VBE_FUNCTION 0x4F
#define SET_VBE_MODE 0x4F02
#define SET_VIDEO_MODE_FUNCTION 0x00

#define VBE_LINEAR_FRAME_BUFFER BIT(14)

#define VBE_MODE_GRAPHICS BIT(10)
#define VBE_MODE_COLOR BIT(8)

int (vbe_set_mode)(uint16_t mode);
int (vbe_text_mode)();

#endif /* __VBE_H */
