#include <lcom/lcf.h>

#include <stdint.h>

int (util_sys_inb)(int port, uint8_t *value) {
    uint32_t new_value;
    if (sys_inb(port, &new_value)) {
        printf("Error reading from the port! \n");
        return 1; 
    }
    *value = (uint8_t) new_value;
    return 0;
}
