#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "kbd.h"
#include "i8042.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int hook_id = 0;
int timer_hook_id = 1;
extern int counter;
uint8_t scancode, status;

int(kbd_test_scan)() {
  
  uint8_t bytes[2];
  uint8_t irq_set;
  counter = 0;
  int i=0;
  message msg;
  int r, ipc_status;

  if(kbd_subscribe_int(&irq_set)) {
    return 1;
  }

  while ( scancode != ESC_BRK ) {
    
    if ( ( r = driver_receive(ANY, &msg, &ipc_status) ) != 0 ) {
      printf("driver_receive failed with: %d", r);
      return 1;
    }

    if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {

      if (msg.m_notify.interrupts & BIT(irq_set)) {
        
        kbc_ih();

        if(scancode == TWO_BYTE) {
          bytes[i] = scancode;
          i++;
          continue;
        }
        bytes[i] = scancode;
        kbd_print_scancode(!(scancode & BR_MK), i+1, bytes);
        i = 0;
        

      }

    }

  }

  if( kbd_unsubscribe_int() != OK ) {
    return 1;
  }

  return kbd_print_no_sysinb(counter);
}



extern int cnt;

int(kbd_test_poll)() {
  
  uint8_t bytes[2];
  int i = 0; 
  uint8_t enable;
  counter = 0;

  do {

    int check = kbc_read_data(&scancode);

    if (check == 2) {
      continue;
    }
    else if (check == 1) {
      printf("Error reading the data");
      break;
    }
    
    if (scancode == TWO_BYTE){
      bytes[i] = scancode;
      i++;
      continue;
    }
    bytes[i] = scancode;
    kbd_print_scancode(!(scancode & BR_MK), i+1, bytes);
    i = 0;


  }while (scancode != ESC_BRK);

  kbd_write_cmd(IN_BUF, READ_CMD_B);
  if (util_sys_inb(OUT_BUF, &enable)) {
    printf("Error reading the data");
    return 1;
  }
  enable = enable | INT;
  kbd_write_cmd(IN_BUF, WRITE_CMD_B);
  kbd_write_cmd(IN_BUF_ARG, enable);


  return kbd_print_no_sysinb(counter);

}

int(kbd_test_timed_scan)(uint8_t n) {
  
  uint8_t bytes[2], timer_irq, kbd_irq;
  int i = 0, r, ipc_status;
  message msg;

  if(kbd_subscribe_int(&kbd_irq)) {
    return 1;
  }

  if(timer_subscribe_int(&timer_irq)) {
    return 1;
  }

  while((scancode != ESC_BRK) && ((cnt/60) < n)) {

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      return 1;
    }

    if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {

      if(msg.m_notify.interrupts & BIT(timer_irq)) {
        timer_int_handler();
      }

      if (msg.m_notify.interrupts & BIT(kbd_irq)) {
        
        kbc_ih();

        if(scancode == TWO_BYTE) {
          bytes[i] = scancode;
          i++;
          continue;
        }
        bytes[i] = scancode;
        kbd_print_scancode(!(scancode & BR_MK), i+1, bytes);
        i = 0;
        cnt = 0;   

      }

    }

  }

  if( kbd_unsubscribe_int() != OK ) {
    return 1;
  }

  if(timer_unsubscribe_int() != OK) {
    return 1;
  }

  return kbd_print_no_sysinb(counter);


}
