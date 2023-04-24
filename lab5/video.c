#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdbool.h>

#include "video.h"

vbe_mode_info_t vmi_p;
static void* video_mem;
static uint16_t hres, vres, bits_per_pixel;

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

int (init_vars)(uint16_t mode) {
    vbe_get_mode_info(mode, &vmi_p);

    hres = vmi_p.XResolution;
    vres = vmi_p.YResolution;
    bits_per_pixel = vmi_p.BitsPerPixel;

    int r;
    struct minix_mem_range mr;
    unsigned int vram_base = vmi_p.PhysBasePtr;
    unsigned int vram_size = hres * vres * bits_per_pixel;

    mr.mr_base = (phys_bytes)vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    }

    video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);
    if (video_mem == MAP_FAILED) {
        panic("couldn't map video memory");
    }

    return 0;
}

int (video_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (int i = 0; i < height && i < vres; i++) video_draw_hline(x, y + i, width, color);
    return 0;
}

int (video_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (int i = 0; i < len && i < hres; i++) video_draw_pixel(x + i, y, color);
    return 0;
}

int (video_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    if (x < 0 || y < 0 || x >= hres || y >= vres) {
        printf("Invalid coordinates: (%d, %d)", x, y);
        return 1;
    }
    
    uint8_t* pixel = (uint8_t*) video_mem + (y * hres + x) * (bits_per_pixel / 8);
    *pixel = color;

    return 0;
}
