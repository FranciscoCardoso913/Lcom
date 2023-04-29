#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"
#include "../lab2/i8254.h"

int timer_hook_id = 1;
int kbd_hook_id = 0;
uint8_t scancode, status;
int cnt = 0;

int kbd_subscribe_int() {
  
  if (sys_irqsetpolicy( KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id)) {
    printf("Error subscribing the keyboard interrupts! \n");
    return 1;
  }
  return 0;
}

int kbd_unsubscribe_int() {
    
    if (sys_irqrmpolicy(&kbd_hook_id)) {
        printf("Error unsubscribing the keyboard interrupts! \n");
        return 1;
    }

    return 0; 

}

int (timer_subscribe_int)() {

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

void (kbc_ih)() {

  if (kbc_read_data(&scancode)) {
    printf("Error reading the data");
  }

}

int kbd_write_cmd(int port, uint8_t cmd) {

  int timecounter = 0;

  do {

    if (kbc_read_status(&status)) {
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

int kbc_read_status(uint8_t *status){

  if (util_sys_inb(STATUS_REG, status)) {
    printf("Error reading the KBC status");
    return 1;
  }

  return 0;
}

int kbc_read_data(uint8_t *data) {

  if (kbc_read_status(&status)) return 1;

  if ((status & OBF) && !(status & AUX)) {

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


void(wait_for_esc)(){

  int ipc_status, r;
  message msg;

  
  while ( scancode != ESC_BRK ) {
    
    if ( ( r = driver_receive(ANY, &msg, &ipc_status) ) != 0 ) {
      printf("driver_receive failed with: %d", r);
      return;
    }

    if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {

      if (msg.m_notify.interrupts & BIT(kbd_hook_id)) {
        
        kbc_ih();

      }

    }

  }

}
