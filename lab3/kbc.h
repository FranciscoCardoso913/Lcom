#ifndef KBC
#define KBC

#include <lcom/lcf.h>
#include <stdint.h>

#include "i8042.h"

int (kbd_subscribe_int)(uint8_t *bit_no); 

int (kbd_unsubscribe_int)();

int kbc_read_status();

#endif // KBC