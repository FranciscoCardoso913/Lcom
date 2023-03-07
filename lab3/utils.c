#include <lcom/lcf.h>
#include <stdint.h>

#ifdef LAB3
uint32_t count_inb_calls = 0;
#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
    *lsb = (uint8_t) (val & 0xff);
    return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
    *msb = (uint8_t) (val >> 8);
    return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
    #ifdef LAB3
    count_inb_calls++;
    #endif
    uint32_t val = (uint32_t) *value;
    if (sys_inb(port, &val)) return 1;
    
    *value = (uint8_t) val;
    return 0;
}
