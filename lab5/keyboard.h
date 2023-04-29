#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lab5.h>
#include <stdint.h>

#define DELAY_US    20000

int kbd_subscribe_int();

int kbd_unsubscribe_int();

int kbd_write_cmd(int port, uint8_t cmd);

int kbc_read_status(uint8_t *status);

int kbc_read_data(uint8_t *data);

void kbd_ih();

void wait_for_esc();

#endif
