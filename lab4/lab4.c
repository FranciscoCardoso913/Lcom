// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include <lcom/lab4.h>
#include<../lab2/timer.c>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/shared/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

uint8_t mouse_hook_id=3;

int counter;
int (mouse_test_packet)(uint32_t cnt) {
  uint32_t c=0;
  uint8_t mouse_irq_set=mouse_hook_id;
  counter=0;
  int r;
  struct packet pp;
  message msg;
  int ipc_status;
 
  if(mouse_subscribe_int(&mouse_irq_set)){
    printf("Error while subscribing the mouse");
    return 1;
  }
  if(enable_stream_mode()){
    printf("Error while enabling stream moode");
    return 1;
  }

  while( c<cnt ) { 
      if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                    printf("driver_receive failed with: %d", r);
                    continue;
                }
      if (is_ipc_notify(ipc_status)) { 
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
            mouse_ih();
            if(counter==3){
              pp = mouse_return_packet();
							mouse_print_packet(&pp);
							c++;
              counter=0;
            }
          break;
          default:
          break; 
        }
  } else {}
  }
  if(mouse_unsubscribe_int()){
    printf("Error while unsubscribing");
    return 1;
  }
  if(disable_stream_mode()){
    printf("Error disabling stream mode");
    return 1;
  }
   return 0;
}

int timer_hook_id=1;
int timer_counter;
int (mouse_test_async)(uint8_t idle_time) {
  uint8_t mouse_irq_set=mouse_hook_id;
  uint8_t timer_irq_set=timer_hook_id;
  counter=0;
  timer_counter=0;
  int r;
  struct packet pp;
  message msg;
  int ipc_status;
 
  if(mouse_subscribe_int(&mouse_irq_set)){
    printf("Error while subscribing the mouse");
    return 1;
  }
  if(timer_subscribe_int(&timer_irq_set)) {
    printf("Error while subscribing the timer");
    return 1;
  }
  if(enable_stream_mode()){
    printf("Error while enabling stream moode");
    return 1;
  }

  while( timer_counter< (int) (sys_hz()*idle_time) ) { 
      if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                    printf("driver_receive failed with: %d", r);
                    continue;
                }
      if (is_ipc_notify(ipc_status)) { 
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
           if (msg.m_notify.interrupts & BIT(mouse_irq_set)) { 
            
            mouse_ih();
            if(counter==3){
              pp = mouse_return_packet();
							mouse_print_packet(&pp);
              counter=0;
              timer_counter=0;
            }
           }
          if (msg.m_notify.interrupts & BIT(timer_irq_set)) {
            timer_int_handler();
          } 
          break;
          default:
          break; 
        }
  } else {}
  }
  if(mouse_unsubscribe_int()){
    printf("Error while unsubscribing");
    return 1;
  }
  if(timer_unsubscribe_int()){
    printf("Error while unsubscribing");
    return 1;
  }
  if(disable_stream_mode()){
    printf("Error disabling stream mode");
    return 1;
  }
   return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  struct mouse_ev *event;
   uint32_t c=0;
  uint8_t mouse_irq_set=mouse_hook_id;
  counter=0;
  int r;
  struct packet pp;
  message msg;
  int ipc_status;
 
  if(mouse_subscribe_int(&mouse_irq_set)){
    printf("Error while subscribing the mouse");
    return 1;
  }
  if(enable_stream_mode()){
    printf("Error while enabling stream moode");
    return 1;
  }

  while( 1 ) { 
      if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                    printf("driver_receive failed with: %d", r);
                    continue;
                }
      if (is_ipc_notify(ipc_status)) { 
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
            mouse_ih();
            if(counter==3){
              pp = mouse_return_packet();
							mouse_print_packet(&pp);
              counter=0;
              event=mouse_detect_event(&pp);
              handle_mouse_event(&event,x_len,tolerance);
            }
          break;
          default:
          break; 
        }
  } else {}
  }
  if(mouse_unsubscribe_int()){
    printf("Error while unsubscribing");
    return 1;
  }
  if(disable_stream_mode()){
    printf("Error disabling stream mode");
    return 1;
  }
   return 0;

}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}