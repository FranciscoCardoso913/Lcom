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
    return;
  }
  
  if (status & KBC_ST_OBF) {
    if (util_sys_inb(OUT_BUF, &scanCode)) {
      fprintf(stderr, "Error while reading output buffer\n");
      err = 1;
      return;                                       
    }

    if (status & (KBC_PAR_ERROR | KBC_TO_ERROR)) {
      fprintf(stderr, "Invalid data read from kbc\n");
      err = 2;
      return;
    }
  }

  err = 0;
}

int kbc_cmd_read(uint8_t *val) {
  int tries = 10;

  while (tries) {
    if (kbc_read_status()) continue;

    if (status & KBC_ST_OBF) {
      util_sys_inb(OUT_BUF, val);

      if (status & (KBC_PAR_ERROR | KBC_TO_ERROR))
        return 1;
      return 0;
    }

    tries--;
    tickdelay(DELAY);
  }
  
  return 1;
}

int kbc_cmd_write(uint8_t port, uint8_t byte) {
  int tries = 10;

  while (tries) {
    if (kbc_read_status()) continue;

    if ((status & KBC_ST_IBF) == 0) {
      sys_outb(port, byte);
      return 0;
    }

    tries--;
    tickdelay(DELAY);
  }

  return 1;
}

uint8_t kbc_command(uint8_t cmd, uint8_t argv[], int argc) {
  uint8_t val;

  /* Write command to kbc */
  if (kbc_cmd_write(IN_BUF, cmd)) {
    fprintf(stderr, "Couldn't write command to kbc\n");
    return 1;
  }
  
  /* Different action depending on command */
  switch(cmd) {
    case READ_CMD:
      if (kbc_cmd_read(&val)) {
        fprintf(stderr, "Error while reading command byte\n");
        err = 1;
        return 1;
      }

      return val;
      break;
    
    case WRITE_CMD:
      for (int i = 0; i < argc; i++) {

        if (kbc_cmd_write(ARG_REG, argv[i])) {
          fprintf(stderr, "Couldn't write command's arg to kbc\n");
          err = 1;
          return 1;
        }
      }
      return 0;
      
      break;

    case CHECK_KBC_CMD:
      if (kbc_cmd_read(&val) || val == KBD_INTERFACE_FAILURE) {
        fprintf(stderr, "Error while reading command byte\n");
        err = 1;
        return 1;
      }
      else if (val == KBD_INTERFACE_SUCCESS)
        return 0;
      else {
        err = 1;
        return 1;
      }
      break;

    case CHECK_KBD_INTERFACE:
      if (kbc_cmd_read(&val) || val != 0) {
        fprintf(stderr, "Error while reading command byte\n");
        err = 1;
        return 1;
      }
      return 0;
      break;
    
    default:
      return 0;
      break;
  }

  return -1;
}
