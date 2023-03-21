#include <lcom/lcf.h>
#include "mouse.h"
#include "i8042.h"
#include "../lab2/i8254.h"
#include "lcom/timer.h"

int32_t hookid_mouse = 0;
struct packet pp;
int idx = 0;
int err;


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
