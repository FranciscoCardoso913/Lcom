#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
void (mouse_ih)();
int (mouse_get_status)(uint8_t* st);
void (mouse_read_data)();
void (mouse_build_packet)(struct packet *pp, uint8_t *bytes);
int (kbc_write)(uint8_t port, uint8_t cmd);
int (mouse_write)(uint8_t cmd);
int (mouse_read)(uint8_t port, uint8_t *data);
int (mouse_restore_int)();
int (mouse_enable_data_report)();
int (mouse_disable_data_report)();
int (mouse_set_stream_mode)();
int (mouse_set_remote_mode)();

#endif /* __MOUSE_H */
