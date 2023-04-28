#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdbool.h>

#include "video.h"

vbe_mode_info_t vmi_p;
static uint8_t* video_mem;
static uint16_t hres, vres, bytes_per_pixel;

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
    memset(&vmi_p, 0, sizeof(vmi_p));
    vbe_get_mode_info(mode, &vmi_p);

    hres = vmi_p.XResolution;
    vres = vmi_p.YResolution;
    bytes_per_pixel = (vmi_p.BitsPerPixel + 7) / 8;
    size_t frame_size = hres * vres * bytes_per_pixel;

    int r;
    struct minix_mem_range mr;
    mr.mr_base = vmi_p.PhysBasePtr;
    mr.mr_limit = mr.mr_base + frame_size;

    if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    }

    video_mem = vm_map_phys(SELF, (void*)mr.mr_base, frame_size);

    if (video_mem == MAP_FAILED) {
        panic("couldn't map video memory");
    }

    return 0;
}

int (video_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (int i = 0; i < height && i < vres; i++) {
        video_draw_hline(x, y + i, width, color);
    }
    return 0;
}

int (video_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (int i = 0; i < len && i < hres; i++) {
        video_draw_pixel(x + i, y, color);
    }
    return 0;
}

int (video_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    if (x < 0 || y < 0 || x >= hres || y >= vres) {
        printf("Invalid coordinates: (%d, %d)", x, y);
        return 1;
    }

    size_t i = (hres * y + x) * bytes_per_pixel;

    if (memcpy(&video_mem[i], &color, bytes_per_pixel) == NULL) {
        return 1;
    }

    return 0;
}

int (video_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step) {
    uint16_t width = hres / no_rectangles;
    uint16_t height = vres / no_rectangles;

    for (int i = 0; i < no_rectangles; i++) {
        for (int j = 0; j < no_rectangles; j++) {
            uint32_t color;

            if (vmi_p.MemoryModel == 0x06) {
                uint32_t R = Red(j, step, first);
                uint32_t G = Green(i, step, first);
                uint32_t B = Blue(j, i, step, first);
                color = direct_mode(R, G, B);

            }
            else {
                color = indexed_mode(j, i, step, first, no_rectangles);
            }

            if (video_draw_rectangle(j * width, i * height, width, height, color)) return 1;
        }
    }

    return 0;
}

int (print_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    xpm_image_t img;
    uint8_t* colors = xpm_load(xpm, XPM_INDEXED, &img);

    for (int h = 0 ; h < img.height ; h++) {
    for (int w = 0 ; w < img.width ; w++, colors++) {
      if (video_draw_pixel(x + w, y + h, *colors) != 0) return 1;
    }
  }
  return 0;
}

int normalize_color(uint32_t color, uint32_t* new_color) {
    if (vmi_p.BitsPerPixel == 32) {
        *new_color = color;
    }
    else {
        *new_color = color & (BIT(vmi_p.BitsPerPixel) - 1);
    }
    return 0;
}

uint32_t(direct_mode)(uint32_t R, uint32_t G, uint32_t B) {
    return (R << vmi_p.RedFieldPosition) | (G << vmi_p.GreenFieldPosition) | (B << vmi_p.BlueFieldPosition);
}

uint32_t(indexed_mode)(uint16_t col, uint16_t row, uint8_t step, uint32_t first, uint8_t n) {
    return (first + (row * n + col) * step) % (1 << vmi_p.BitsPerPixel);
}

uint32_t(Red)(unsigned j, uint8_t step, uint32_t first) {
    return (R(first) + j * step) % (1 << vmi_p.RedMaskSize);
}

uint32_t(Green)(unsigned i, uint8_t step, uint32_t first) {
    return (G(first) + i * step) % (1 << vmi_p.GreenMaskSize);
}

uint32_t(Blue)(unsigned j, unsigned i, uint8_t step, uint32_t first) {
    return (B(first) + (i + j) * step) % (1 << vmi_p.BlueMaskSize);
}

uint32_t(R)(uint32_t first) {
    return ((1 << vmi_p.RedMaskSize) - 1) & (first >> vmi_p.RedFieldPosition);
}

uint32_t(G)(uint32_t first) {
    return ((1 << vmi_p.GreenMaskSize) - 1) & (first >> vmi_p.GreenFieldPosition);
}

uint32_t(B)(uint32_t first) {
    return ((1 << vmi_p.BlueMaskSize) - 1) & (first >> vmi_p.BlueFieldPosition);
}
