#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "mouse.h"

int hook_mouse = 2;
uint8_t size = 0;
struct packet pp;

static enum mouse_state state = INIT;
static int16_t dx = 0, dy = 0;

int(mouse_subscribe_int)(int *bit_no) {
  *bit_no = BIT(hook_mouse);
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_mouse);
}

int(mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_mouse);
}

int(mouse_get_status)(uint8_t *st) {
  return util_sys_inb(KBC_ST_REG, st);
}

bool wait_ibf_empty() {
  uint8_t status, tries = 10;

  while (tries--) {
    if (mouse_get_status(&status))
      continue;
    if (!(status & KBC_IN_FULL))
      return true;
  }
  return false;
}

bool wait_obf_full() {
  uint8_t status, tries = 10;

  while (tries--) {
    if (mouse_get_status(&status))
      continue;
    if (status & KBC_OUT_FULL)
      return true;
  }
  return false;
}

int issue_mouse_command(uint8_t args[], int argc) {
  uint8_t tries = 3, acks = 0, mouse_response;

  while (acks != argc && tries) {

    if (!wait_ibf_empty())
      return 1;
    sys_outb(KBC_IN_BUF_UP, MOUSE_WRITE_DATA);

    if (!wait_ibf_empty())
      return 1;
    sys_outb(KBC_IN_BUF_DN, args[acks]);

    if (!wait_obf_full())
      return 1;

    util_sys_inb(KBC_OUT_BUF, &mouse_response);

    if (mouse_response == MOUSE_ACK)
      acks++;
    else {
      acks = 0;
      tries--;
    }
  }

  if (acks == argc)
    return 0;

  else {
    printf("Couldn't write command to mouse after %d tries\n", tries);
    return 1;
  }
}

int(mouse_write)(uint8_t cmd) {
  uint8_t args[1];
  args[0] = cmd;
  return issue_mouse_command(args, 1);
}

void(mouse_ih)() {
  uint8_t st, byte;
  if (size > 2)
    size = 0;
  if (mouse_get_status(&st) != F_OK)
    return;
  if (st & (KBC_OUT_FULL | MOUSE_DATA)) {
    util_sys_inb(KBC_OUT_BUF, &byte);
    if (st & KBC_ERROR)
      return;
    if (size == 0 && !(byte & MOUSE_SYNC_BIT))
      return;
    pp.bytes[size++] = byte;
  }
}

void(mouse_build_packet)() {
  pp.lb = pp.bytes[0] & MOUSE_LB;
  pp.rb = pp.bytes[0] & MOUSE_RB;
  pp.mb = pp.bytes[0] & MOUSE_MB;
  pp.delta_x = pp.bytes[0] & MOUSE_MSBX ? pp.bytes[1] | 0xff00 : pp.bytes[1];
  pp.delta_y = pp.bytes[0] & MOUSE_MSBY ? pp.bytes[2] | 0xff00 : pp.bytes[2];
  pp.x_ov = pp.bytes[0] & MOUSE_XOV;
  pp.y_ov = pp.bytes[0] & MOUSE_YOV;
}

bool(mouse_handle_gesture)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance) {
  static void (*state_func[])(struct mouse_ev * event, uint8_t x_len, uint8_t tolerance) = {
    mouse_handle_init,
    mouse_handle_first_line,
    mouse_handle_vertex,
    mouse_handle_second_line,
  };

  state_func[state](event, x_len, tolerance);
  return state == END;
}

void(mouse_handle_init)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance) {
  dx = dy = 0;
  if (event->type == LB_PRESSED) {
    state = FIRST_LINE;
  }
}

void(mouse_handle_first_line)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance) {
  if (event->type == LB_RELEASED) {
    if ((dx >= x_len) && (dy / dx) >= 1) {
      dx = dy = 0;
      state = VERTEX;
      return;
    }
    else {
      state = INIT;
      return;
    }
  }
  else if (event->type == MOUSE_MOV) {
    if (event->delta_x <= 0 || event->delta_y <= 0) {
      if (abs(event->delta_x) > tolerance || abs(event->delta_y) > tolerance) {
        state = INIT;
        return;
      }
    }
    if (event->delta_x != 0 && (event->delta_y) / (event->delta_x) <= 1) {
      state = INIT;
    }
    else {
      dx += event->delta_x;
      dy += event->delta_y;
    }
  }
  else {
    state = INIT;
  }
}

void(mouse_handle_vertex)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance) {
  if (event->type == RB_PRESSED) {
    if (abs(dx) > tolerance || abs(dy) > tolerance) {
      state = INIT;
    }
    else {
      dx = dy = 0;
      state = SECOND_LINE;
      return;
    }
  }
  else if (event->type == MOUSE_MOV) {
    dx += event->delta_x;
    dy += event->delta_y;
  }
  else if (event->type == LB_PRESSED) {
    dx = dy = 0;
    state = FIRST_LINE;
    return;
  }
  else {
    state = INIT;
  }
}

void(mouse_handle_second_line)(struct mouse_ev *event, uint8_t x_len, uint8_t tolerance) {
  if (event->type == RB_RELEASED) {
    if (dx >= x_len && dy / dx <= -1) {
      state = END;
      return;
    }
    else {
      state = INIT;
      return;
    }
  }
  else if (event->type == MOUSE_MOV) {
    if (event->delta_x <= 0 || event->delta_y >= 0) {
      if (abs(event->delta_x) > tolerance || abs(event->delta_y) > tolerance) {
        state = INIT;
        return;
      }
    }
    if (event->delta_x != 0 && (event->delta_y) / (event->delta_x) >= -1) {
      state = INIT;
    }
    else {
      dx += event->delta_x;
      dy += event->delta_y;
    }
  }
  else {
    state = INIT;
  }
}
