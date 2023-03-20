#ifndef MOUSE
#define MOUSE

#include <lcom/lcf.h>

int issue_mouse_command(uint8_t *res, uint8_t argv[], int argc);

int read_status(uint8_t *res);

int read_outbuffer(uint8_t *res);

int subscribe_mouse(uint8_t *irqset);

int unsubscribe_mouse();

#endif
