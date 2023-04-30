#ifndef VIDEO
#define VIDEO

#include <lcom/lcf.h>

int graphics_mode_init(uint16_t mode);

int map_vram(unsigned int physBasePtr);

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

bool out_of_bounds(uint16_t x, uint16_t y);

uint32_t get_color_field_mask(uint8_t mask_size, uint8_t lsb);

uint32_t get_specific_color_field(uint32_t color, uint8_t mask_size, uint8_t lsb);

uint32_t get_pattern_indexed_color(uint8_t no_rectangles, uint32_t first, uint8_t step, unsigned row, unsigned col);

uint32_t get_pattern_direct_color(uint8_t no_rectangles, uint32_t first, uint8_t step, unsigned row, unsigned col);

int draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step);

int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

#endif
