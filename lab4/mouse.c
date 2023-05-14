#include <lcom/lcf.h>
#include "mouse.h"
#include "i8042.h"
#include "../lab2/i8254.h"
#include "lcom/timer.h"

int32_t hookid_mouse = 0;
struct packet pp;
int idx = 0;
int err;
int x, y;


int subscribe_mouse(uint8_t *bitno) {
  *bitno = hookid_mouse;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookid_mouse)) {
    printf("Error while subscribing to mouse interrupts\n");
    return 1;
  }
  return 0;
}

int unsubscribe_mouse() {
  if (sys_irqrmpolicy(&hookid_mouse)) {
    printf("Error while unsubscribing from mouse interrupts\n");
    return 1;
  }
  return 0;
}

int read_status(uint8_t *res) {
  if (util_sys_inb(STATUS_REG, res)) {
    printf("Error while reading kbc status register in read_status\n");
    return 1;
  }
  return 0;
}

bool wait_ibf_empty() {
  int tries = 10;
  uint8_t status;

  while (tries--) {
    if (read_status(&status)) {
      continue;
    }

    if (!(status & KBC_ST_IBF))
      return true;
  }

  printf("Timed out in wait_ibf_empty\n");
  return false;
}

bool wait_obf_full() {
  int tries = 10;
  uint8_t status;

  while (tries--) {
    if (read_status(&status)) {
      continue;
    }

    if (status & KBC_ST_OBF)
      return true;
  }

  printf("Timed out in wait_obf_full\n");
  return false;
}

/**
 * Issues a command to the mouse
 * args: array with the command and the respective arguments if appliable. The first element of the array must be a valid command
 * argc: number of elements inside args
*/
int issue_mouse_command(uint8_t args[], int argc) {
  uint8_t tries = 3, acks = 0, mouse_response;

  while (acks != argc && tries) {

    if (!wait_ibf_empty())
      return TIMEOUT_ERR;
    
    sys_outb(IN_BUF, WRITE_BYTE_MOUSE);

    if (!wait_ibf_empty())
      return TIMEOUT_ERR;
    
    sys_outb(ARG_REG, args[acks]);
    
    if (!wait_obf_full())
      return TIMEOUT_ERR;

    util_sys_inb(OUT_BUF, &mouse_response);

    if (mouse_response == MOUSE_ACK) {
      acks++;
    }
    else {
      acks = 0;
      tries--;
    }
  }

  if (acks == argc) {
    return 0;
  }
  else {
    printf("Couldn't write command to mouse after %d tries\n", tries);
    return 1;
  }
}

bool sync_mouse(uint8_t byte) {
  return idx == 0 && !(byte & MOUSE_ALWAYS_ON);
}

void (mouse_ih)() {
  uint8_t status, byte;

  if (idx == 3) idx = 0;

  if (read_status(&status)) {
    printf("Error when reading status\n");
    return;
  }

  if (status & (KBC_ST_OBF | KBC_ST_AUX)) {

    util_sys_inb(OUT_BUF, &byte);

    if (status & (KBC_PAR_ERROR | KBC_TO_ERROR)) {
      printf("Parity or timeout error when reading mouse data\n");
      return;
    }

    if (idx == 0 && !(byte & MOUSE_ALWAYS_ON)) {
      printf("Mouse wasn't synched\n");
      return;
    }
    pp.bytes[idx++] = byte;
  }
}

struct packet mouse_return_packet() {
  pp.lb = pp.bytes[0] & MOUSE_LB;
  pp.rb = pp.bytes[0] & MOUSE_RB;
  pp.mb = pp.bytes[0] & MOUSE_MB;
  pp.delta_x = pp.bytes[0] & MOUSE_MSBX_DELTA ? pp.bytes[1] | 0xff00 : pp.bytes[1];
  pp.delta_y = pp.bytes[0] & MOUSE_MSBY_DELTA ? pp.bytes[2] | 0xff00 : pp.bytes[2];
  pp.x_ov = pp.bytes[0] & MOUSE_X_OVFL;
  pp.y_ov = pp.bytes[0] & MOUSE_Y_OVFL;
  return pp;
}

uint8_t count_buttons_pressed(struct packet *pp) {
  int buttons = 0;

  if (pp->lb)
    buttons++;

  if (pp->rb)
    buttons++;
  
  if (pp->mb)
    buttons++;
  
  return buttons;
}

struct mouse_ev* mouse_get_event(struct packet *pp) {
  struct mouse_ev *event = malloc(sizeof *event);
  event->delta_x = pp->delta_x;
  event->delta_y = pp->delta_y;

  static struct packet lastpp = {.rb = false, .lb = false, .mb = false};
  uint8_t lastButtonsPressed, thisButtonsPressed;

  lastButtonsPressed = count_buttons_pressed(&lastpp);
  thisButtonsPressed = count_buttons_pressed(pp);

  if (lastButtonsPressed == thisButtonsPressed) {
    event->type = MOUSE_MOV;
  }
  else if (lastButtonsPressed < thisButtonsPressed && thisButtonsPressed == 1) {
    if (pp->lb)
      event->type = LB_PRESSED;
    else if (pp->rb)
      event->type = RB_PRESSED;
    else
      event->type = BUTTON_EV;
  }
  else if (lastButtonsPressed > thisButtonsPressed && thisButtonsPressed == 0) {
    if (lastpp.lb)
      event->type = LB_RELEASED;
    else if (lastpp.rb)
      event->type = RB_RELEASED;
    else
      event->type = BUTTON_EV;
  }
  else
    event->type = BUTTON_EV;

  lastpp = *pp;

  return event;
}

mouse_state mouse_handle_init(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance) {
  x = y = 0;

  switch(ev->type) {
    case LB_PRESSED:
      return FIRST_LINE;
    default:
      return INIT;
  }
}

mouse_state mouse_handle_first_line(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance) {
  bool x_over_tolerance, y_over_tolerance;

  switch(ev->type) {
    case MOUSE_MOV:
      x_over_tolerance = ev->delta_x < 0 && ev->delta_x > tolerance;
      y_over_tolerance = ev->delta_y < 0 && ev->delta_y > tolerance;

      if (x_over_tolerance || y_over_tolerance)
        return INIT;
      
      x += ev->delta_x; y += ev->delta_y;

      if (x != 0 && y / x < 1)
        return INIT;
      
      return FIRST_LINE;

    case LB_RELEASED:
      if (x >= x_len) {
        x = y = 0;
        return VERTEX;
      }
      return INIT;

    default:
      return INIT;
  }
}

mouse_state mouse_handle_vertex(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance) {
  switch(ev->type) {
    case RB_PRESSED:
      x = y = 0;
      return SECOND_LINE;

    case MOUSE_MOV:
      if (abs(ev->delta_x) > tolerance || abs(ev->delta_y) > tolerance)
        return INIT;
      
      x += ev->delta_x; y += ev->delta_y;

      if (abs(x) > tolerance || abs(y) > tolerance)
        return INIT;
      
      return VERTEX;

    case LB_PRESSED:
      x = y = 0;
      return FIRST_LINE;

    default:
      return INIT;
  }
}

mouse_state mouse_handle_second_line(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance) {
  bool x_over_tolerance, y_over_tolerance;

  switch(ev->type) {
    case MOUSE_MOV:
      x_over_tolerance = ev->delta_x < 0 && ev->delta_x > tolerance;
      y_over_tolerance = ev->delta_y < 0 && ev->delta_y > tolerance;

      if (x_over_tolerance || y_over_tolerance)
        return INIT;
      
      x += ev->delta_x; y += ev->delta_y;

      if (x != 0 && y / x > -1)
        return INIT;
      
      return SECOND_LINE;

    case RB_RELEASED:
      if (x >= x_len) {
        return END;
      }
      return INIT;

    default:
      return INIT;
  }
}

bool mouse_handle_event(struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance) {
  static mouse_state st = INIT;
  static mouse_state (*st_funcs[]) (struct mouse_ev *ev, uint8_t x_len, uint8_t tolerance) = {
    mouse_handle_init,
    mouse_handle_first_line,
    mouse_handle_vertex,
    mouse_handle_second_line,
  };

  st = st_funcs[st](ev, x_len, tolerance);

  if (st == END)
    return true;
  return false;
}
