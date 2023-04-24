// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab4.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "mouse.h"
#include "i8042.h"
#include "../lab2/i8254.h"

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

extern int err;
extern int idx;

int (mouse_test_packet)(uint32_t cnt) {
	message msg;
	int r, ipc_status;
	uint8_t irqset, args[1];
	struct packet pp;

	if (subscribe_mouse(&irqset)) {
		return 1;
	}

  args[0] = 0xF4;
	if (issue_mouse_command(args, 1)) {
		printf("Error when enabling data reporting\n");
		return 1;
	}

	while (cnt) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset)) {
						mouse_ih();

						if (idx == 3) {
							pp = mouse_return_packet();
							mouse_print_packet(&pp);
							cnt--;
						}
					}
          break;
        default:
          break;
      }
    }
    else {}
  }

  args[0] = 0xF5;
	if (issue_mouse_command(args, 1)) {
		printf("Error when disabling data reporting\n");
		return 1;
	}

	if (unsubscribe_mouse()) {
		return 1;
	}

	return 0;
}

extern int counter;

int (mouse_test_async)(uint8_t idle_time) {
  message msg;
  int r, ipc_status;
  uint8_t irqset_timer, irqset_mouse, args[1];
  struct packet pp;

  if (subscribe_mouse(&irqset_mouse)) {
    return 1;
  }

  if (timer_subscribe_int(&irqset_timer)) {
    return 1;
  }

  args[0] = 0xF4;
  if (issue_mouse_command(args, 1)) {
    printf("Error when enabling data reporting\n");
    return 1;
  }

  while (counter < (int) (idle_time*sys_hz())) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset_mouse)) {
						mouse_ih();

						if (idx == 3) {
							pp = mouse_return_packet();
							mouse_print_packet(&pp);
							counter = 0;
						}
					}

          if (msg.m_notify.interrupts & BIT(irqset_timer)) {
            timer_int_handler();
          }
          break;
        default:
          break;
      }
    }
    else {}
  }

  args[0] = 0xF5;
  if (issue_mouse_command(args, 1)) {
    printf("Error when disabling data reporting\n");
    return 1;
  }

  if (timer_unsubscribe_int()) {
    return 1;
  }

  if (unsubscribe_mouse()) {
    return 1;
  }

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  message msg;
	int r, ipc_status;
	uint8_t irqset, args[1];
	struct packet pp;
  bool mouseEvent = false;
  struct mouse_ev *ev;

	if (subscribe_mouse(&irqset)) {
		return 1;
	}

  args[0] = 0xF4;
	if (issue_mouse_command(args, 1)) {
		printf("Error when enabling data reporting\n");
		return 1;
	}

	while (1) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset)) {
						mouse_ih();

						if (idx == 3) {
							pp = mouse_return_packet();
							mouse_print_packet(&pp);
              ev = mouse_detect_event(&pp);
              mouseEvent = true;
						}
					}
          break;
        default:
          break;
      }
    }
    else {}
    
    if (mouseEvent) {
      if (mouse_handle_event(ev, x_len, tolerance))
        break;
      mouseEvent = false;
    }
  }

  args[0] = 0xF5;
	if (issue_mouse_command(args, 1)) {
		printf("Error when disabling data reporting\n");
		return 1;
	}

	if (unsubscribe_mouse()) {
		return 1;
	}

	return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
