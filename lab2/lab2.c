#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>


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
  /* To be implemented by the students */
  uint8_t status;
  if (timer_get_conf(timer, &status)) {
    fprintf(stderr, "Error in timer_get_conf\n");
    return 1;
  }
  if (timer_display_conf(timer, status, field)) {
    fprintf(stderr, "Error in timer_display_conf\n");
    return 1;
  }
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  if (timer_set_frequency(timer, freq)) {
    fprintf(stderr, "Error in timer_set_frequency\n");
    return 1;
  }

  return 0;
}

int counter;
int hookid = 0;

int(timer_test_int)(uint8_t time) {
  int r;
  uint8_t irqset;
  counter = 0;

  int ipc_status;
  message msg;

  if (timer_subscribe_int(&irqset)) {
    fprintf(stderr, "Error when subscribing\n");
    return 1;
  }

  while (counter < time*60) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(irqset)) {
            timer_int_handler();
            if (counter % (int) sys_hz() == 0) timer_print_elapsed_time();
          }
          break;
        default:
          break;
      }
    }
    else {}
  }

  if (timer_unsubscribe_int()) {
    fprintf(stderr, "Error when unsubscribing\n");
    return 1;
  }

  return 0;
}
