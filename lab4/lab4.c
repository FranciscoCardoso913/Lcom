#include <lcom/lcf.h>
// why
#include <lcom/lab4.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "mouse.h"

extern uint32_t counter_timer;
extern uint8_t size, read_byte;
extern struct packet pp;

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
  int irq_set, ipc_status;
  message msg;
  uint8_t r;

  if (mouse_subscribe_int(&irq_set) != F_OK)
    return 1;
  if (mouse_write(MOUSE_ENABLE_DATA) != F_OK)
    return 1;

  while (cnt) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != F_OK)
      continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if (size == 3) {
              mouse_build_packet();
              mouse_print_packet(&pp);
              cnt--;
            }
          }
        default: break;
      }
    }
  }

  if (mouse_write(MOUSE_DISABLE_DATA) != F_OK)
    return 1;
  if (mouse_unsubscribe_int() != F_OK)
    return 1;

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  int irq_set, ipc_status;
  message msg;
  uint8_t r, irq_timer;

  if (mouse_subscribe_int(&irq_set) != F_OK)
    return 1;
  if (timer_subscribe_int(&irq_timer) != F_OK)
    return 1;
  if (mouse_write(MOUSE_ENABLE_DATA) != F_OK)
    return 1;

  uint32_t freq = sys_hz();

  while (counter_timer < idle_time * freq) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != F_OK)
      continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if (size == 3) {
              mouse_build_packet();
              mouse_print_packet(&pp);
            }
            counter_timer = 0;
          }
          if (msg.m_notify.interrupts & irq_timer) {
            timer_int_handler();
          }
        default: break;
      }
    }
  }

  if (mouse_write(MOUSE_DISABLE_DATA) != F_OK)
    return 1;
  if (mouse_unsubscribe_int() != F_OK)
    return 1;
  if (timer_unsubscribe_int() != F_OK)
    return 1;

  return 0;
}

struct mouse_ev* mouse_event;

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int irq_set, ipc_status;
  message msg;
  uint8_t r;

  if (mouse_subscribe_int(&irq_set) != F_OK)
    return 1;
  if (mouse_write(MOUSE_ENABLE_DATA) != F_OK)
    return 1;

  bool done = false;

  while (!done) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != F_OK)
      continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if (size == 3) {
              mouse_build_packet();
              mouse_event = mouse_detect_event(&pp);
              done = mouse_handle_gesture(mouse_event, x_len, tolerance);
              mouse_print_packet(&pp);
            }
          }
        default: break;
      }
    }
  }

  if (mouse_write(MOUSE_DISABLE_DATA) != F_OK)
    return 1;
  if (mouse_unsubscribe_int() != F_OK)
    return 1;

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
