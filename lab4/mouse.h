#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lab4.h>
#include <stdint.h>

#define DELAY_US    20000

int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

int mouse_write_cmd(int port, uint8_t cmd);

int mouse_read_status(uint8_t *status);

int mouse_read_data(uint8_t *data);

#endif
