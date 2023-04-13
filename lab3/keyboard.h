#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"

int (keyboard_subscribe_int)(uint8_t *bit_no);
int (keyboard_unsubscribe_int)();
void (keyboard_ih)();
int (keyboard_get_status)(uint8_t* st);
void (keyboard_read_data)();
int (keyboard_read)(uint8_t port, uint8_t *data);
int (keyboard_write)(uint8_t port, uint8_t cmd);
int (keyboard_restore_int)();

#endif /* __KEYBOARD_H */
