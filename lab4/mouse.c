#include "mouse.h"
#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

int hook_mouse = 2;
uint8_t size = 0, read_byte;

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = BIT(hook_mouse);
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_mouse);
}

int(mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_mouse);
}

int(mouse_get_status)(uint8_t *st) {
  return util_sys_inb(KBC_ST_REG, st);
}

void(mouse_read_data)() {
  util_sys_inb(KBC_OUT_BUF, &read_byte);
}

int(kbc_write)(uint8_t port, uint8_t cmd) {
  uint8_t st, wait = 5;

  while (wait--) {
    if (mouse_get_status(&st) || st & KBC_IN_FULL) {
      tickdelay(WAIT_KBC);
      continue;
    }
    sys_outb(port, cmd);
    return 0;
  }

  return 1;
}

int(mouse_write)(uint8_t cmd) {
  uint8_t st;

  do {
    if (kbc_write(KBC_IN_BUF_UP, MOUSE_WRITE_DATA) != F_OK)
      return 1;
    if (kbc_write(KBC_OUT_BUF, cmd) != F_OK)
      return 1;
    if (mouse_get_status(&st) != F_OK)
      return 1;
  } while (st != MOUSE_ACK);

  return 0;
}

int(mouse_read)(uint8_t port, uint8_t *data) {
  return util_sys_inb(port, data);
}

void(mouse_ih)() {
  uint8_t st;
  if (mouse_get_status(&st) != F_OK || st & KBC_ERROR) return;
  if (st & KBC_OUT_FULL) mouse_read_data();
}

void(mouse_build_packet)(struct packet *pp, uint8_t bytes[3]) {
  pp->bytes[0] = bytes[0];
  pp->bytes[1] = bytes[1];
  pp->bytes[2] = bytes[2];
  pp->lb = bytes[0] & MOUSE_LB;
  pp->rb = bytes[0] & MOUSE_RB;
  pp->mb = bytes[0] & MOUSE_MB;
  pp->x_ov = bytes[0] & MOUSE_XOV;
  pp->y_ov = bytes[0] & MOUSE_YOV;
  pp->delta_x = bytes[1];
  pp->delta_y = bytes[2];
  if (bytes[0] & MOUSE_MSBX) pp->delta_x |= 0xFF00;
  if (bytes[0] & MOUSE_MSBY) pp->delta_y |= 0xFF00;
  pp->delta_x = -pp->delta_x;
  pp->delta_y = -pp->delta_y;
}
