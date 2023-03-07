#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

int counter;
int hook_id = 0;

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

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  
  if (timer < 0 || timer > 2) {
    printf("Invalid timer number! \n"); 
    return 1;
  }

  uint8_t time; 

  int timer_conf = timer_get_conf(timer, &time);

  if (timer_conf != 0) {
    printf("Error reading the timer configuration! \n");
    return 1;
  }
  
  timer_display_conf(timer, time, field);
  
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  
  if (timer < 0 || timer > 2) {
    printf("Invalid timer number! \n"); 
    return 1;
  }

  return timer_set_frequency(timer, freq);
}

int(timer_test_int)(uint8_t time) {
  
  counter = 0;
  uint8_t bit_no;
  int r, ipc_status;
  message msg;

 

  if (timer_subscribe_int(&bit_no)) {
    printf("Error subscribing the timer interrupts! \n");
    return 1;
  }  


  while (counter < time*60) {
    
    
    if ( ( r = driver_receive(ANY, &msg, &ipc_status) ) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(bit_no)) {
            timer_int_handler();
            if (counter % (int) sys_hz() == 0) {
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break;
      }
    }
    
  }

  if (timer_unsubscribe_int()) {
    printf("Error unsubscribing the timer interrupts! \n");
    return 1;
  }

  return 0;
  
}
