#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdbool.h>

#include "video.h"

static int hres, vres, bits_per_pixel;

int (video_set_mode)(uint16_t mode) {
    reg86_t r;
    memset(&r, 0, sizeof(r));

    r.intno = VBE_VID_INT;
    r.ax = VBE_SET_MODE;
    r.bx = mode | VBE_LINEAR_BUFFER;

    if (sys_int86(&r) != F_OK) {
        printf("sys_int86() failed\n");
        return 1;
    }

    return 0;
}

int (text_mode)() {
    reg86_t r;
    memset(&r, 0, sizeof(r));

    r.intno = VBE_VID_INT;
    r.ah = BIOS_SET_MODE;
    r.al = TEXT_MODE;

    if (sys_int86(&r) != F_OK) {
        printf("sys_int86() failed\n");
        return 1;
    }

    return 0;
}

int (video_mode)() {
    return video_set_mode(VBE_VIDEO_MODE);
}

