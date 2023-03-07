#include <lcom/lcf.h>
#include "keyboard.h"

#include <stdint.h>

extern uint8_t scancode, size, bytes[2];
extern int hook_id, ret;

int (kbd_subscribe_int)(uint8_t *bit_no) {
    *bit_no = hook_id;
    if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)) return 1;
    return 0;
}

int (kbd_unsubscribe_int)() {
    if (sys_irqrmpolicy(&hook_id)) return 1;
    return 0;
}


void (kbc_ih)() {
    ret = 0;
    size = 0;

    // if parity or timeout error
    if (status & (PAR_ERR | TO_ERR)) {
        ret = 1;
        return;
    }

    // read data from output buffer
    if (util_sys_inb(OUT_BUF, &scancode)) {
        ret = 1;
        return;
    }
}

