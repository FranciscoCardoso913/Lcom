#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
int (util_sys_inb)(int port, uint8_t *value) {
    uint32_t aux;
    sys_inb(port,&aux); 
    *value =(uint8_t) aux;
    return 0;
}



