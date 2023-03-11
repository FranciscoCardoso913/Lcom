#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"


int main(int argc, char *argv[]) {  
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

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

int hookid, err;
extern int cnt;
uint8_t status, scanCode;

int (kbd_test_scan)() {
  int r, ipc_status;
  uint8_t irqset, idx = 0;
  message msg;
  hookid = 0;
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
              bool isMakecode = idx == 0 ? !(scanCodes[0] & BIT(7)) : !(scanCodes[1] & BIT(7));
              kbd_print_scancode(isMakecode, idx + 1, scanCodes);
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
  kbd_print_no_sysinb(cnt);

  return 0;
}