#include <lcom/lcf.h>
// why
#include <lcom/lab4.h>

#include <stdbool.h>
#include <stdint.h>

#include "mouse.h"

extern uint8_t size, read_byte;
uint8_t bytes[3];

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

int(mouse_test_packet)(uint32_t cnt) {
  uint8_t irq_set;

  if (mouse_write(MOUSE_ENABLE_DATA) != F_OK) return 1;

  if (mouse_subscribe_int(&irq_set) != F_OK) return 1;

  int ipc_status;
  message msg;
  uint8_t r;
  bool start = false;

  while (cnt) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != F_OK) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();
            
            if (read_byte & MOUSE_SYNC_BIT) start = true;
            
            if (start) bytes[size++] = read_byte;

            if (size == 3) {
              struct packet pp;
              mouse_build_packet(&pp, bytes);
              mouse_print_packet(&pp);continue;
              size = 0;
              cnt--;
              start = false;
            }
          }
        default: break;
      }
    }
  }

  if (mouse_unsubscribe_int() != F_OK) return 1;

  if (mouse_write(MOUSE_DISABLE_DATA) != F_OK) return 1;

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  printf("%s(): under construction\n", __func__);
  return 1;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  printf("%s(): under construction\n", __func__);
  return 1;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
