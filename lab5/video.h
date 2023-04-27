#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>
#include <lcom/lab5.h>

#define VBE_VID_INT 0x10

#define VBE_MODE_INFO 0x4F01
#define VBE_SET_MODE 0x4F02
#define VBE_GET_MODE 0x4F03
#define VBE_MODE_SUPPORTED 0x4F00
#define VBE_FUNCTION 0x4F
#define VBE_VIDEO_MODE 0x0105
#define SET_VBE_MODE 0x4F02
#define BIOS_SET_MODE 0x00
#define TEXT_MODE 0x03

#define VBE_LINEAR_BUFFER BIT(14)

#define VBE_MODE_GRAPHICS BIT(10)
#define VBE_MODE_COLOR BIT(8)

int (video_set_mode)(uint16_t mode);
int (video_mode)();
int (set_text_mode)();
int (init_vars)(uint16_t mode);
int (video_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (video_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (video_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (video_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step);

#endif /* __VIDEO_H */
