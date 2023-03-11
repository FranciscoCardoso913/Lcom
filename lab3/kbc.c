#include <lcom/lcf.h>
#include <stdint.h>

#include "kbc.h"
#include "i8042.h"

extern int hookid, err;
extern uint8_t status, scanCode;

int (kbd_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hookid;
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookid)) {
    fprintf(stderr, "Error while setting kbd subscription\n");
    return 1;
  }

  return 0;
}

int (kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hookid)) {
    fprintf(stderr, "Error while removing kbd subscription\n");
    return 1;
  }

  return 0;
}

int kbc_read_status() {
  return util_sys_inb(STAT_REG, &status);
}

void (kbc_ih)() {
  if (kbc_read_status()) {
    fprintf(stderr, "Error when reading kbc status\n");
    err = 1;
  }
  
  if (status & OUT_BUF_FULL) {
    util_sys_inb(OUT_BUF, &scanCode);

    if (status & (KBC_PAR_ERROR | KBC_TO_ERROR)) {
      fprintf(stderr, "Invalid data read from kbc\n");
      err = 2;
    }
  }
  
  err = 0;
}