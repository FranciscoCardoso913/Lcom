#include <lcom/lcf.h>
#include "keyboard.h"

#include <stdint.h>
#include <stdbool.h>

int hook_keyboard = 1;
uint8_t size = 0, bytes[2];

int (keyboard_subscribe_int)(uint8_t *bit_no) {
    *bit_no = BIT(hook_keyboard);
    return sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_keyboard);
}

int (keyboard_unsubscribe_int)() {
    return sys_irqrmpolicy(&hook_keyboard);
}

int (keyboard_get_status)(uint8_t* st) {
    return util_sys_inb(KBC_ST_REG, st);
}

void (keyboard_read_data)(){
    util_sys_inb(KBC_OUT_BUF, &bytes[size]);
}

void (keyboard_ih)() {
    uint8_t st;
    if (keyboard_get_status(&st) != F_OK) return;

    if (st & KBC_ERROR & MOUSE_DATA) return;

    if (st & KBC_OUT_FULL) keyboard_read_data();
}

int (keyboard_write)(uint8_t port, uint8_t cmd) {
    uint8_t st, wait = 5;

    while (wait--) {
        if (keyboard_get_status(&st) || st & KBC_IN_FULL) {
            tickdelay(WAIT_KBC);
            continue;
        }
        sys_outb(port, cmd);
        return 0;
    }

    return 1;
}

int (keyboard_read)(uint8_t port, uint8_t *data) {
    return util_sys_inb(port, data);
}

int (keyboard_restore_int)() {
    uint8_t cmd;

    keyboard_write(KBC_IN_BUF_UP, KBC_READ_CMD);
    keyboard_read(KBC_OUT_BUF, &cmd);
    
    cmd |= 1;

    keyboard_write(KBC_IN_BUF_UP, KBC_WRITE_CMD);
    keyboard_write(KBC_IN_BUF_DN, cmd);

    return 0;
}
