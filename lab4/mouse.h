#ifndef MOUSE
#define MOUSE
#include <lcom/lcf.h>
#include <stdint.h>

#include "../lab3/i8042.h"
enum mouse_state {
    INIT,
    FIRST_LINE,
    VERTEX,
    SECOND_LINE,
    END
};


int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();
void (mouse_ih)();
bool (enable_stream_mode)();
bool (disable_stream_mode)();
bool (wait_ibf_empty)(); 
struct packet ( mouse_return_packet)();
void (mouse_handle_init)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance);
void (mouse_handle_first_line)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance);
void (mouse_handle_vertex)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance);
void (mouse_handle_second_line)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance);



#endif