#ifndef __KBD_H
#define __KBD_H

#include <lcom/lab3.h>
#include <stdint.h>

#define DELAY_US    20000

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

int kbd_write_cmd(int port, uint8_t cmd);

int kbc_read_status(uint8_t *status);

int kbc_read_data(uint8_t *data);

#endif
