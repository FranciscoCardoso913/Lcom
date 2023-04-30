#ifndef VIDEO
#define VIDEO

#include <lcom/lcf.h>

int graphics_mode_init(uint16_t mode);

int map_vram(unsigned int physBasePtr);

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

bool out_of_bounds(uint16_t x, uint16_t y);

#endif
