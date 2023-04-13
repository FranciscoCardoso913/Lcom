#include <lcom/lcf.h>
#include <stdint.h>

unsigned int sys_inb_calls = 0;

int (util_get_LSB)(uint16_t val, uint8_t *lsb) {
    *lsb = (uint8_t) (val & 0xff);
    return 0;
}

int (util_get_MSB)(uint16_t val, uint8_t *msb) {
    *msb = (uint8_t) (val >> 8);
    return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
    u_int32_t v = (u_int32_t) *value;
    sys_inb_calls++;
    
    if (sys_inb(port, &v)) return 1;
    
    *value = (uint8_t) v;
    return 0;
}
