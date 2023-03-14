#include <lcom/lcf.h>

#include "kbd.h"
#include "i8042.h"

extern int hook_id;
extern uint8_t scancode;


int kbd_subscribe_int(uint8_t *bit_no) {
  *bit_no = hook_id; 
  if (sys_irqsetpolicy( KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)) {
    printf("Error subscribing the keyboard interrupts! \n");
    return 1;
  }
  return 0;
}

int kbd_unsubscribe_int() {
    
    if (sys_irqrmpolicy(&hook_id)) {
        printf("Error unsubscribing the keyboard interrupts! \n");
        return 1;
    }

    return 0; 

}

void (kbc_ih)() {

  uint8_t st = 0;

  if(util_sys_inb(STATUS_REG, &st)) {
    printf("Error reading the KBD status");
    return;
  } 

  /* Checks if there is something to read */
  if(OBF & st) {
    
    if(util_sys_inb(OUT_BUF, &scancode)){
      printf("Error reading the buffer");
      return; 
    }

    if(st & (TIMEOUT_ERR | PARITY_ERR)) {
      printf("Error: Invalid data");
      return;
    }

    if (st & AUX) {
      printf("Trying to read mouse interrupts");
      return;
    }

  }

}
