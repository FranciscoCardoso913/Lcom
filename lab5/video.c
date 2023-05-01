#include <lcom/lcf.h>
#include "video.h"
#include "video_macros.h"
#include "math.h"

static void *video_mem;
static unsigned h_res;
static unsigned v_res;
static unsigned bits_per_pixel;
static uint8_t red_mask_size, green_mask_size, blue_mask_size;
static uint8_t red_lsb_position, green_lsb_position, blue_lsb_position;
static bool is_indexed = false;
static xpm_image_t img;
static uint8_t *bitmap;

void* (vg_init)(uint16_t mode) {
  vbe_mode_info_t vbe_mode_info;

  vbe_get_mode_info(mode, &vbe_mode_info);

  h_res = vbe_mode_info.XResolution;
  v_res = vbe_mode_info.YResolution;
  bits_per_pixel = vbe_mode_info.BitsPerPixel;
  red_mask_size = vbe_mode_info.RedMaskSize;
  green_mask_size = vbe_mode_info.GreenMaskSize;
  blue_mask_size = vbe_mode_info.BlueMaskSize;
  red_lsb_position = vbe_mode_info.RedFieldPosition;
  green_lsb_position = vbe_mode_info.GreenFieldPosition;
  blue_lsb_position = vbe_mode_info.BlueFieldPosition;

  if (mode == 0x105) is_indexed = true;

  if (map_vram(vbe_mode_info.PhysBasePtr))
    return NULL;

  if (graphics_mode_init(mode))
    return NULL;

  return video_mem;
}

int map_vram (unsigned int physBasePtr) {
  struct minix_mem_range mr;
  unsigned int vram_base = physBasePtr;
  size_t vram_size = h_res * v_res * (bits_per_pixel + 7)/8;
  int r;				    

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size;  

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    printf("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED) {
    printf("Couldn't map video memory\n");
    return 1;
  }

  return 0;
}

int graphics_mode_init(uint16_t mode) {
  reg86_t r86;
  
  memset(&r86, 0, sizeof(r86));

  r86.intno = INT_INTERFACE;
  r86.ah = AH_REG;
  r86.al = AL_SET_VBE_MODE;
  r86.bx = mode | LINEAR_BUFFER;
  
  if ( sys_int86(&r86) != OK ) {
    printf("\tvg_exit(): sys_int86() failed \n");
    return 1;
  }

  return 0;
}

bool out_of_bounds(uint16_t x, uint16_t y) {
  if (x < 0 || x >= h_res || y < 0 || y >= v_res) {
    printf("Position out of bounds\n");
    return true;
  }

  return false;
}

int (vg_draw_rectangle) (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (out_of_bounds(x, y))
    return 1;

  for (unsigned line = y; line < y + height && line < v_res; line++) {
    if (vg_draw_hline(x, line, width, color))
      return 1;
  }

  return 0;
}

int (vg_draw_hline) (uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (out_of_bounds(x, y))
    return 1;

  for (unsigned pixel = x; pixel < x + len && pixel < h_res; pixel++) {
    if (vg_draw_pixel(pixel, y, color))
      return 1;
  }

  return 0;
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  uint8_t byteOffset = (bits_per_pixel + 7)/8;

  if (out_of_bounds(x, y))
    return 1;
  
  if (bits_per_pixel != 32)
    color &= MASK_N_BITS(bits_per_pixel);
  
  uint8_t *pixelPtr = (uint8_t *) video_mem + (x + y * h_res) * byteOffset;

  memcpy(pixelPtr, &color, byteOffset);

  return 0;
}

uint32_t get_color_field_mask(uint8_t mask_size, uint8_t lsb) {
  uint32_t withFieldMask, withoutFieldMask;

  if (mask_size + lsb == 32)
    withFieldMask = MASK_32_BITS;
  else
    withFieldMask = MASK_N_BITS(lsb + mask_size);
  
  if (lsb != 0)
    withoutFieldMask = MASK_N_BITS(lsb);
  else
    withoutFieldMask = 0;

  return withFieldMask - withoutFieldMask;

}

uint32_t get_specific_color_field(uint32_t color, uint8_t mask_size, uint8_t lsb) {
  return (get_color_field_mask(mask_size, lsb) & color) >> lsb;
}

uint32_t get_pattern_indexed_color(uint8_t no_rectangles, uint32_t first, uint8_t step, unsigned row, unsigned col) {
  return (first + (row * no_rectangles + col) * step) % BIT(bits_per_pixel);  
}

uint32_t get_pattern_direct_color(uint8_t no_rectangles, uint32_t first, uint8_t step, unsigned row, unsigned col) {
  uint32_t red_field, green_field, blue_field;

  red_field = get_specific_color_field(first, red_mask_size, red_lsb_position);

  green_field = get_specific_color_field(first, green_mask_size, green_lsb_position);

  blue_field = get_specific_color_field(first, blue_mask_size, blue_lsb_position);

  red_field = (red_field + col * step) % BIT(red_mask_size);

  green_field = (green_field + row * step) % BIT(green_mask_size);

  blue_field = (blue_field + (col + row) * step) % BIT(blue_mask_size);

  return (red_field << red_lsb_position) |
          (green_field << green_lsb_position) |
          (blue_field << blue_lsb_position);
}

int draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step) {
  unsigned rect_side_x = h_res / no_rectangles;
  unsigned rect_side_y = v_res / no_rectangles;

  for (unsigned row = 0; row < no_rectangles; row++) {
    for (unsigned col = 0; col < no_rectangles; col++) {

      uint32_t color = is_indexed ? 
        get_pattern_indexed_color(no_rectangles, first, step, row, col) :
        get_pattern_direct_color(no_rectangles, first, step, row, col);
      
      if (vg_draw_rectangle(rect_side_x * col, rect_side_y * row, rect_side_x, rect_side_y, color))
        return 1;
    }
  }

  return 0;
}

int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y) {
  for (uint16_t line = 0; line < img.height; line++) {
    for (uint16_t col = 0; col < img.width; col++) {
      if (vg_draw_pixel(x + col, y + line, bitmap[line * img.width + col]))
        return 1;
    }
  }

  return 0;
}

int erase_xpm(uint16_t x, uint16_t y) {
  if (vg_draw_rectangle(x, y, img.width, img.height, COLOR_BLACK))
    return 1;
  return 0;
}

void xpm_init(xpm_map_t xpm) {
  bitmap = xpm_load(xpm, XPM_INDEXED, &img);
}
