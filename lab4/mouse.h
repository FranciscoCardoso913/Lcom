#ifndef MOUSE
#define MOUSE

#include <lcom/lcf.h>

int issue_mouse_command(uint8_t args[], int argc);

int read_status(uint8_t *res);

bool wait_ibf_empty();

bool wait_obf_full();

int subscribe_mouse(uint8_t *bitno);

int unsubscribe_mouse();

bool sync_mouse(uint8_t byte);

struct packet mouse_return_packet();

#endif
