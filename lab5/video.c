#include <lcom/lcf.h>
#include "video.h"
#include "video_macros.h"

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
