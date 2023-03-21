#ifndef KBC
#define KBC

#include <lcom/lcf.h>
#include <stdint.h>

#include "i8042.h"

int (kbd_subscribe_int)(uint8_t *bit_no); 

int (kbd_unsubscribe_int)();

int kbc_read_status();

int kbc_cmd_write(uint8_t port, uint8_t byte);

int kbc_cmd_read(uint8_t *val);

uint8_t kbc_command(uint8_t cmd, uint8_t arg[], int argc);

#endif // KBC