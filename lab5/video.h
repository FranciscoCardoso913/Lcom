#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>
#include <stdint.h>

int (video_graphic_init)();

int (video_set_mode)(uint16_t mode);

void (set_mem)(uint16_t mode);

#endif
