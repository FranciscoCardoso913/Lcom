#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdbool.h>

#include "vbe.h"

int (vbe_set_mode)(uint16_t mode) {
    reg86_t r;
    memset(&r, 0, sizeof(r));

    r.intno = BIOS_VIDEO_SERVICES;
    r.ax = VBE_SET_MODE;
    r.bx = mode | VBE_LINEAR_FRAME_BUFFER;

    if (sys_int86(&r) != F_OK) {
        printf("sys_int86() failed\n");
        return 1;
    }

    return 0;
}

int (vbe_text_mode)() {
    reg86_t r;
    memset(&r, 0, sizeof(r));

    r.intno = BIOS_VIDEO_SERVICES;
    r.ah = SET_VIDEO_MODE_FUNCTION;
    r.al = 0x03;

    if (sys_int86(&r) != F_OK) {
        printf("sys_int86() failed\n");
        return 1;
    }

    return 0;
}
