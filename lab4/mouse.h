#ifndef MOUSE
#define MOUSE
#include <lcom/lcf.h>
#include <stdint.h>

#include "../lab3/i8042.h"

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();
void (mouse_ih)();
bool (enable_stream_mode)();
bool (disable_stream_mode)();
bool (wait_ibf_empty)(); 
struct packet ( mouse_return_packet)();


#endif