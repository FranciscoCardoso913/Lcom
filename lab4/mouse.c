#include <lcom/lcf.h>

#include "mouse.h"
#include "i8042.h"
#include "../lab2/i8254.h"

extern int timer_hook_id;
extern int mouse_hook_id;
extern uint8_t scancode;
extern uint8_t status;
int cnt = 0;

int mouse_subscribe_int(uint8_t *bit_no) {
  *bit_no = mouse_hook_id; 
  if (sys_irqsetpolicy( MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id)) {
    printf("Error subscribing the mouse interrupts! \n");
    return 1;
  }
  return 0;
}

int mouse_unsubscribe_int() {
    
    if (sys_irqrmpolicy(&mouse_hook_id)) {
        printf("Error unsubscribing the mouse interrupts! \n");
        return 1;
    }

    return 0; 

}

int (timer_subscribe_int)(uint8_t *bit_no) {

  *bit_no = timer_hook_id;

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id)) {
    printf("Error setting the policy! \n");
    return 1;
  }

  return 0;

}

int (timer_unsubscribe_int)() {
  
  if (sys_irqrmpolicy(&timer_hook_id)) {
    printf("Error removing the policy! \n");
    return 1;
  }

  return 0;
}

int mouse_write_cmd(int port, uint8_t cmd) {

  int timecounter = 0;

  do {

    if (mouse_read_status(&status)) {
      return 1;
    }

  
    if ((status & IBF) == 0) {
      if (sys_outb(port, cmd) != OK) {
        printf("Error writing the command");
        return 1;
      }
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY_US));
    timecounter++;

  } while (timecounter < 10);

  return 1;

}

int mouse_read_status(uint8_t *status){

  if (util_sys_inb(STATUS_REG, status)) {
    printf("Error reading the mouse status");
    return 1;
  }

  return 0;
}

int mouse_read_data(uint8_t *data) {

  if (mouse_read_status(&status)) return 1;

  if ((status & OBF) && (status & AUX)) {

    if (util_sys_inb(OUT_BUF, data)) {
      printf("Error reading the buffer");
      return 1;
    }

    if (status & (TIMEOUT_ERR | PARITY_ERR)) {
      printf("Error: Invalid data");
      return 2;
    }

    return 0;
  }


  tickdelay(micros_to_ticks(DELAY_US));

  return 2;

}

void (timer_int_handler)() {
  cnt++;
}