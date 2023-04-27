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

extern int timer_counter;
uint8_t byte, status;

int (mouse_test_packet)(uint32_t cnt) {
    
  uint8_t irq_set;
  message msg;
  int ipc_status, r, idx = 0;
  struct packet pp;



  if (mouse_subscribe_int(&irq_set)) {
    printf("Error subscribing the mouse interrupts! \n");
    return 1;
  } 

  if (mouse_command_handler(ENABLE_DATA_REP)) {
    printf("Error enabling the stream! \n");
    return 1;
  }


  while (cnt > 0) {
    
    
    if ( ( r = driver_receive(ANY, &msg, &ipc_status) ) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status) && (_ENDPOINT_P(msg.m_source) == HARDWARE)) {
      if (msg.m_notify.interrupts & irq_set) {
        
        mouse_ih();
        
        if (idx == 0 && (byte & BIT(3)) == 0) {
          continue;
        }

        pp.bytes[idx++] = byte;

        if (idx == 3) {
          idx = 0;
          build_packet(&pp);
          mouse_print_packet(&pp);
          cnt--;
        }    

      }
    }

  }


  if (mouse_command_handler(DISABLE_DATA_REP)) {
    printf("Error disabling the stream! \n");
    return 1;
  }

  if (mouse_unsubscribe_int()) {
    printf("Error unsubscribing the mouse interrupts! \n");
    return 1;
  }

  return 0;

}

int (mouse_test_async)(uint8_t idle_time) {
    

  uint8_t irq_set_mouse, irq_set_timer;
  message msg;
  int ipc_status, r, idx = 0;
  struct packet pp;
  int freq = sys_hz();

  if (mouse_subscribe_int(&irq_set_mouse))  {
    printf("Error subscribing the mouse interrupts! \n");
    return 1;
  }

  if (timer_subscribe_int(&irq_set_timer)) {
    printf("Error subscribing the timer interrupts! \n");
    return 1;
  }

  if(mouse_command_handler(ENABLE_DATA_REP)) {
    printf("Error enabling the stream! \n");
    return 1;
  }

  util_sys_inb(0x60, &byte);
  byte = 0;

  while(timer_counter < idle_time*freq) {

    if ( ( r = driver_receive(ANY, &msg, &ipc_status) ) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status) && (_ENDPOINT_P(msg.m_source) == HARDWARE)) {
      
      if (msg.m_notify.interrupts & irq_set_timer) {
        
        timer_ih();

      }

      if (msg.m_notify.interrupts & irq_set_mouse) {

        timer_counter = 0;

        mouse_ih();
        
        if (idx == 0 && (byte & BIT(3)) == 0) {
          continue;
        }

        pp.bytes[idx++] = byte;

        if (idx == 3) {
          idx = 0;
          build_packet(&pp);
          mouse_print_packet(&pp);
  
        }    

      }

    }
  }

  if(mouse_command_handler(DISABLE_DATA_REP)) {
    printf("Error disabling the stream! \n");
    return 1;
  }

  if(mouse_unsubscribe_int()) {
    printf("Error unsubscribing the mouse interrupts! \n");
    return 1;
  }

  if (timer_unsubscribe_int()) {
    printf("Error unsubscribing the timer interrupts! \n");
    return 1;
  }

  return 0;

}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
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

  if (mouse_read_data(&byte)) {
    printf("Error reading the data from mouse! \n");
  }

}
