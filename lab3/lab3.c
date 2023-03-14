#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include <../lab2/i8254.h>

#include "kbc.h"


int main(int argc, char *argv[]) {  
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/shared/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


/* Global variables */

int hookid_kbc, hookid_timer, err = 0;
extern int cnt;
int counter = 0;
uint8_t status, scanCode;

int (kbd_test_scan)() {
  int r, ipc_status;
  uint8_t irqset, idx = 0;
  message msg;
  hookid_kbc = 0;
  bool ignore = false;
  uint8_t scanCodes[2];

  if (kbd_subscribe_int(&irqset)) return 1;

  while (scanCode != ESC_CODE) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset)) {
            kbc_ih();

            if (err == 1)
              continue;
            else if (err == 2)
              ignore = true;
            
            if (scanCode == TWO_BYTE_CODE) {
              scanCodes[idx++] = scanCode;
            }
            else {
              scanCodes[idx] = scanCode;
              if (!ignore) {
                bool isMakecode = idx == 0 ? !(scanCodes[0] & BIT(7)) : !(scanCodes[1] & BIT(7));
                kbd_print_scancode(isMakecode, idx + 1, scanCodes);
                ignore = false;
              }
              idx = 0;
            }
          }
          break;
        default:
          break;
      }
    }
    else {}
  }

  if (kbd_unsubscribe_int()) return 1;

  if (kbd_print_no_sysinb(cnt)) {
    fprintf(stderr, "Error when calling kbd_print_no_sysinb\n");
  }

  return 0;
}

int (kbd_test_poll)() {
  uint8_t argv[1];
  uint8_t scanCodes[2], idx = 0;
  bool ignore = false;

  if (err == 1) {
    return 1;
  }

  do {
    if (kbc_read_status()) {
      fprintf(stderr, "Error while reading kbc status\n");
      continue;
    }

    if (status & KBC_ST_OBF) {
      if (util_sys_inb(OUT_BUF, &scanCode)) {
        continue;
      }

      if (status & (KBC_PAR_ERROR | KBC_TO_ERROR))
        ignore = true;
      
      if (scanCode == TWO_BYTE_CODE) {
        scanCodes[idx++] = scanCode;
      }
      else {
        scanCodes[idx] = scanCode;
        if (!ignore) {
          bool isMakecode = idx == 0 ? !(scanCodes[0] & BIT(7)) : !(scanCodes[1] & BIT(7));
          kbd_print_scancode(isMakecode, idx + 1, scanCodes);
          ignore = false;
        }
        idx = 0;
      }
    }
  } while (scanCode != ESC_CODE);

  // Restore command byte
  uint8_t commandByte = kbc_command(READ_CMD, argv, 0);
  commandByte |= ENABLE_KBC_INTERRUPT;
  argv[0] = commandByte;

  // Keep on trying to restore interrupts
  while (kbc_command(WRITE_CMD, argv, 1))
    fprintf(stderr, "Error while restoring kbc keyboard interrupts\n");

  if (kbd_print_no_sysinb(cnt)) {
    fprintf(stderr, "Error when executing kbd_print_no_sysinb\n");
  }
  
  return 0;
}

int (kbd_test_timed_scan)(uint8_t n) {
  message msg;
  hookid_timer = 0; hookid_kbc = 1;
  uint8_t irqset_timer, irqset_kbc;
  uint8_t bytes[2], idx = 0;
  int r, ipc_status, lastUpdated = 0;

  if (timer_subscribe_int(&irqset_timer)) {
    printf("Error while subscribing to timer\n");
    return 1;
  }

  if (kbd_subscribe_int(&irqset_kbc)) {
    printf("Error while subscribing to kbd\n");
    return 1;
  }

  while (scanCode != ESC_CODE && (counter - lastUpdated) < (int) (n*sys_hz())) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset_kbc)) {
            kbc_ih();

            if (err == 1)
              continue;
            else if (err == 2) {
              idx = 0;
              continue;
            }
            
            if (scanCode == TWO_BYTE_CODE) {
              bytes[idx++] = scanCode;
            }
            else {
              bytes[idx] = scanCode;
              bool isMakecode = idx == 0 ? !(bytes[0] & BIT(7)) : !(bytes[1] & BIT(7));
              kbd_print_scancode(isMakecode, idx + 1, bytes);
              lastUpdated = counter;
              idx = 0;
            }
          }

          if (msg.m_notify.interrupts & BIT(irqset_timer)) {
            timer_int_handler();
          }
          
          break;
        default:
          break;
      }
    }
    else {}
  }

  if (timer_unsubscribe_int()) {
    printf("Error while unsubscribing to timer\n");
    return 1;
  }

  if (kbd_unsubscribe_int()) {
    printf("Error while unsubscribing to kbd\n");
    return 1;
  }

  return 0;
}