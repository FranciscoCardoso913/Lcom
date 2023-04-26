// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "../lab2/i8254.h"
#include "mouse.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int hook_id = 12;
int timer_hook_id = 1;
extern int counter;
uint8_t scancode, status;

int (mouse_test_packet)(uint32_t cnt) {
    
  uint8_t irq_set;
  message msg;
  int ipc_status, r;


  if (mouse_subscribe_int(&irq_set)) {
    printf("Error subscribing the mouse interrupts! \n");
    return 1;
  } 

  while (cnt > 0) {
    
    
    if ( ( r = driver_receive(ANY, &msg, &ipc_status) ) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    //Enables stream

    if (is_ipc_notify(ipc_status) && (_ENDPOINT_P(msg.m_source) == HARDWARE)) {
      if (msg.m_notify.interrupts & BIT(bit_no)) {
            


      }
    }

  }

  //disables stream

  if (mouse_unsubscribe_int()) {
    printf("Error unsubscribing the mouse interrupts! \n");
    return 1;
  }

  return 0;

}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)() {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}

void (mouse_ih)(){

  if (mouse_read_data(&scancode)) {
    printf("Error reading the data");
  }

}
