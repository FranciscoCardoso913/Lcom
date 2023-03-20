#include <lcom/lcf.h>

#include <stdint.h>
int cnt;
int (util_sys_inb)(int port, uint8_t *value) {
    uint32_t aux;
    sys_inb(port,&aux);
    cnt++;  
    *value =(uint8_t) aux;
    return 0;
}



