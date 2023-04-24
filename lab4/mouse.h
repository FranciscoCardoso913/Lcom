#ifndef MOUSE
#define MOUSE

#include <lcom/lcf.h>

typedef enum {
  INIT,
  FIRST_LINE,
  VERTEX,
  SECOND_LINE,
  END
} mouse_state;

int issue_mouse_command(uint8_t args[], int argc);

int read_status(uint8_t *res);

bool wait_ibf_empty();

bool wait_obf_full();

int subscribe_mouse(uint8_t *bitno);

int unsubscribe_mouse();

bool sync_mouse(uint8_t byte);

struct packet mouse_return_packet();

bool mouse_handle_event(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance);

mouse_state mouse_handle_init(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance);

mouse_state mouse_handle_first_line(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance);

mouse_state mouse_handle_vertex(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance);

mouse_state mouse_handle_second_line(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance);

#endif
