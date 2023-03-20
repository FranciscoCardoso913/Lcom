// IMPORTANT: you must include the following line in all your C files
#include <stdbool.h>
#include <lcom/lab4.h>
#include <lcom/lcf.h>

#include "mouse.h"
#include "i8042.h"
#include <stdint.h>
#include <stdio.h>

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

uint8_t byte;
int hookid = 0;
extern int err;

int(mouse_test_packet)(uint32_t cnt) {
	/* To be completed */
	message msg;
  struct packet pp;
	int ipc_status, r, counter = 0;
	uint8_t irqset, *cmd_return = NULL, bytes[3];
	uint8_t args[5];

  args[0] = ENABLE_DATA_REP;

  if (subscribe_mouse(&irqset)) {
    return 1;
  }

	if (issue_mouse_command(cmd_return, args, 1)) {
    fprintf(stderr, "Unexepected error while issuing mouse command\n");
    return 1;
  }

	while (cnt) {

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
		}  

		if (is_ipc_notify(ipc_status)) {
		switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
			if (msg.m_notify.interrupts & BIT(irqset)) {
        err = 0;
        mouse_ih();

        if (err == 1) {
          continue;
        }

        else if (err == 2) {
          counter = 0;
          continue;
        }

        else {
          if (counter == 0 && !(byte & BIT(3))) {
            counter = 0;
            continue;
          }

          bytes[counter++] = byte;

          // Write packet
          if (counter == 3) {
            memcpy(pp.bytes, bytes, sizeof bytes);
            pp.rb = bytes[0] & BIT(1);
            pp.mb = bytes[0] & BIT(2);
            pp.lb = bytes[0] & BIT(0);
            pp.delta_x = bytes[0] & BIT(4) ? bytes[1] | 0xFF : bytes[1];
            pp.delta_y = bytes[0] & BIT(4) ? bytes[2] | 0xFF : bytes[2];
            pp.x_ov = bytes[0] & BIT(6);
            pp.y_ov = bytes[0] & BIT(7);

            mouse_print_packet(&pp);
            counter = 0;
            cnt--;
          }
        }
			}
			  break;
			default:
			  break;
		}
		}
		else {}
	}

  args[0] = DISABLE_DATA_REP;
  if (issue_mouse_command(cmd_return, args, 1)) {
    fprintf(stderr, "Unexepected error while issuing mouse command\n");
    return 1;
  }

  if (unsubscribe_mouse()) {
    return 1;
  }

	return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  /* To be completed */
  printf("%s(%u): under construction\n", __func__, idle_time);
  return 1;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
