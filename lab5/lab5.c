// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "video.h"
#include "i8042.h"
#include "i8254.h"
#include "kbc.h"
#include "video_macros.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/shared/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (graphics_mode_init(mode))
    return 1;

  sleep(delay);

  if (vg_exit())
    return 1;

  return 0;
}

uint8_t status, scanCode;
int counter = 0;
int err = 0;

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
uint16_t width, uint16_t height, uint32_t color) {
  int r, ipc_status;
  uint8_t irqset, idx = 0;
  message msg;
  bool ignore = false;
  uint8_t scanCodes[2];

  if (vg_init(mode) == NULL)
    return 1;
  
  if (vg_draw_rectangle(x, y, width, height, color))
    return 1;

  if (kbd_subscribe_int(&irqset)) return 1;

  while (scanCode != ESC_CODE) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset)) {
            kbc_ih();

            if (err == 1)
              continue;
            else if (err == 2)
              ignore = true;
            
            if (scanCode == TWO_BYTE_CODE) {
              scanCodes[idx++] = scanCode;
            }
            else {
              scanCodes[idx] = scanCode;
              if (!ignore) {
                ignore = false;
              }
              idx = 0;
            }
          }
          break;
        default:
          break;
      }
    }
    else {}
  }

  if (kbd_unsubscribe_int()) return 1;

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  int r, ipc_status;
  uint8_t irqset, idx = 0;
  message msg;
  bool ignore = false;
  uint8_t scanCodes[2];

  if (vg_init(mode) == NULL)
    return 1;
  
  if (draw_pattern(no_rectangles, first, step))
    return 1;

  if (kbd_subscribe_int(&irqset)) return 1;

  while (scanCode != ESC_CODE) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset)) {
            kbc_ih();

            if (err == 1)
              continue;
            else if (err == 2)
              ignore = true;
            
            if (scanCode == TWO_BYTE_CODE) {
              scanCodes[idx++] = scanCode;
            }
            else {
              scanCodes[idx] = scanCode;
              if (!ignore) {
                ignore = false;
              }
              idx = 0;
            }
          }
          break;
        default:
          break;
      }
    }
    else {}
  }

  if (kbd_unsubscribe_int()) return 1;

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  int r, ipc_status;
  uint8_t irqset, idx = 0;
  message msg;
  bool ignore = false;
  uint8_t scanCodes[2];

  if (vg_init(INDEXED_MODE) == NULL)
    return 1;

  xpm_init(xpm);

  if (vg_draw_xpm(xpm, x, y))
    return 1;

  if (kbd_subscribe_int(&irqset)) return 1;

  while (scanCode != ESC_CODE) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset)) {
            kbc_ih();

            if (err == 1)
              continue;
            else if (err == 2)
              ignore = true;
            
            if (scanCode == TWO_BYTE_CODE) {
              scanCodes[idx++] = scanCode;
            }
            else {
              scanCodes[idx] = scanCode;
              if (!ignore) {
                ignore = false;
              }
              idx = 0;
            }
          }
          break;
        default:
          break;
      }
    }
    else {}
  }

  if (kbd_unsubscribe_int()) return 1;

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  int r, ipc_status;
  uint8_t irqset_timer, irqset_kbc, idx = 0;
  message msg;
  bool ignore = false;
  uint8_t scanCodes[2];
  uint16_t step = speed < 0 ? 1 : speed, x = xi, y = yi;
  bool vert_mov = xi == xf;

  if (xi != xf && yi != yf) {
    printf("Not a straigt line\n");
    return 1;
  }

  if (speed == 0) {
    printf("Speed is 0 (animation would not run)\n");
    return 1;
  }

  if (vg_init(INDEXED_MODE) == NULL)
    return 1;

  xpm_init(xpm);
  
  if (vg_draw_xpm(xpm, xi, yi)) return 1;

  if (timer_set_frequency(TIMER0_IRQ, fr_rate))
    return 1;
  
  if (timer_subscribe_int(&irqset_timer))
    return 1;

  if (kbd_subscribe_int(&irqset_kbc)) return 1;

  while (scanCode != ESC_CODE) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0) {
      fprintf(stderr, "driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & BIT(irqset_kbc)) {
            kbc_ih();

            if (err == 1)
              continue;
            else if (err == 2)
              ignore = true;
            
            if (scanCode == TWO_BYTE_CODE) {
              scanCodes[idx++] = scanCode;
            }
            else {
              scanCodes[idx] = scanCode;
              if (!ignore) {
                ignore = false;
              }
              idx = 0;
            }
          }

          if (msg.m_notify.interrupts & BIT(irqset_timer)) {
            timer_int_handler();

            if (counter % (speed < 0 ? abs(speed) : 1) == 0) {
              if (!(x == xf && y == yf)) erase_xpm(x, y);

              if (!vert_mov) {
                if (x < xf) {
                  x +=  xf - x < step ? xf - x : step;
                }
                else if (x > xf) {
                  x -=  x - xf < step ? x - xf : step;
                }
              }
              else {
                if (y < yf) {
                  y +=  yf - y < step ? yf - y : step;
                }
                else if (y > yf) {
                  y -=  y - yf < step ? y - yf : step;
                }
              }

              if (vg_draw_xpm(xpm, x, y)) return 1;
            }
          }
          break;
        default:
          break;
      }
    }
    else {}
  }

  if (timer_unsubscribe_int()) return 1;

  if (kbd_unsubscribe_int()) return 1;

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
