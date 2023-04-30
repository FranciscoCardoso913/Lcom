#include "video.h"
#include <lcom/lab5.h>

int (video_set_mode(uint16_t mode)) {   
    reg86_t r86;
   
  /* Specify the appropriate register values */
  
    memset(&r86, 0, sizeof(r86));	/* zero the structure */

    r86.intno = 0x10; /* BIOS video services */
    r86.ax = 0x4f02;    /* Set Video Mode function */
    r86.bx = mode | BIT(14);    /* 80x25 text mode */
    
    /* Make the BIOS call */

    if(sys_int86(&r86)) {
        printf("\tvg_exit(): sys_int86() failed \n");
        return 1;
    }
    return 0;
}

vbe_mode_info_t vmi_p;
static uint8_t *video_mem;   
uint16_t h_res, v_res, bits_per_pixel;      

int set_mem(uint16_t mode){

    struct minix_mem_range mr;
    size_t vram_size;  
    int r;	

    memset(&vmi_p, 0, sizeof(vmi_p));
    vbe_get_mode_info(mode, &vmi_p);

    h_res = vmi_p.XResolution;
    v_res = vmi_p.YResolution;
    bits_per_pixel = vmi_p.BitsPerPixel+7;

    vram_size = h_res * v_res * (bits_per_pixel/8);

    mr.mr_base = vmi_p.PhysBasePtr;	
    mr.mr_limit = mr.mr_base + vram_size;  
    			
    if(OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");

    return 0;
}

int (video_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

    if(x > h_res || y > v_res || x < 0 || y < 0) {
        printf("Error in video_draw_pixel(): invalid coordinates\n");
        return 1;
    }

    size_t k = (x + (y * h_res)) * (bits_per_pixel/8);
                
    if (memcpy(&video_mem[k], &color, bits_per_pixel/8) == NULL) {
        printf("Error in video_draw_pixel(): memcpy failed\n");
        return 1;
    }

    return 0;

}

int (video_draw_line)(uint16_t x, uint16_t y, uint16_t lenght, uint32_t color) {

    for (int i = 0; i < lenght && i < h_res; i++) {
        if(video_draw_pixel(x + i, y, color)) {
            return 1;
        }
    }

    return 0;

}

int (video_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    
    for (int i = 0; i < height && i< v_res ; i++ ) {

        if(video_draw_line(x,y+i,width,color)) return 1;

    }

    return 0;

}
