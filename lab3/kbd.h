#ifndef __KBD_H
#define __KBD_H

#include <lcom/lab3.h>
#include <stdint.h>


int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

#endif
