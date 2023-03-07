#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>



int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/shared/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/lab2/output.txt");

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
  /* To be implemented by the students */
  uint8_t st;
  if(timer_get_conf(timer,&st)){
    printf("Error in timer_get_config");
    return 1;
  };
  if(timer_display_conf(timer,st,field)){
     printf("Error in timer_display_conf");
    return 1;
  };
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if(timer_set_frequency(timer,freq)) return 1;
  return 0;
}
int counter;
int hook_id;
int(timer_test_int)(uint8_t time) {
  int r;
  counter = 0;
  int ipc_status;
  message msg;
  if(timer_subscribe_int(&hook_id)){
    printf("Error while subscribing.");
    return 1;
  }
  while( counter<time*60 ) { /* You may want to use a different condition */
  /* Get a request message. */
  if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
    printf("driver_receive failed with: %d", r);
    continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification *11:*/
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(hook_id)) { 
            timer_int_handler();
            if(counter%(int) sys_hz()==0){
              timer_print_elapsed_time();
            }
         }
        break;
        default:
        break; /* no other notifications expected: do nothi19: */
      }
      } else {}
    }
     if(timer_unsubscribe_int()){
    printf("Error while subscribing.");
    return 1;
  }


  return 0;
}
