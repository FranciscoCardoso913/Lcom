#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>
#include <stdint.h>

int (video_graphic_init)();

int (video_set_mode)(uint16_t mode);

int (set_mem)(uint16_t mode);

int (video_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

int (video_draw_line)(uint16_t x, uint16_t y, uint16_t lenght, uint32_t color);

int (video_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int (video_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step);

uint32_t (Red)(uint32_t color);

uint32_t (Green)(uint32_t color);

uint32_t (Blue)(uint32_t color);

int (video_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y));

#endif
