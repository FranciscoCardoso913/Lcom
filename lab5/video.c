#include <lcom/lcf.h>
#include "video.h"
#include "video_macros.h"
#include "math.h"

static void *video_mem;
static unsigned h_res;
static unsigned v_res;
static unsigned bits_per_pixel;

int counterrr = 0;

void* (vg_init)(uint16_t mode) {
  vbe_mode_info_t vbe_mode_info;

  vbe_get_mode_info(mode, &vbe_mode_info);

  h_res = vbe_mode_info.XResolution;
  v_res = vbe_mode_info.YResolution;
  bits_per_pixel = vbe_mode_info.BitsPerPixel;

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
    color &= COLOR_BITS(bits_per_pixel);
  
  uint8_t *pixelPtr = (uint8_t *) video_mem + (x + y * h_res) * byteOffset;

  memcpy(pixelPtr, &color, byteOffset);

  return 0;
}
