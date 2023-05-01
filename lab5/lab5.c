// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include <stdint.h>
#include <stdio.h>

#include "video.h"
#include "keyboard.h"
#include <lcom/timer.h>

#include "i8254.h"
#include "i8042.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

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

  if(video_set_mode(mode)) {
    printf("Error in vg_init()\n");
    return 1;
  }

  sleep(delay);

  if(vg_exit()) {
    printf("Error in vg_exit()\n");
    return 1;
  }

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  
  if(set_mem(mode)) {
    printf("Could not set the memory\n");
    return 1;
  }

  if (video_set_mode(mode)) {
    printf("Error in vg_init()\n");
    return 1;
  }

  if(video_draw_rectangle(x, y, width, height, color)) {
    printf("Error in video_draw_rectangle()\n");
    return 1;
  }

  wait_for_esc();

  if(vg_exit()) {
    printf("Error in vg_exit()\n");
    return 1;
  }

  return 0;

}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  
  if(set_mem(mode)){
    printf("Could not set the memory\n");
    return 1;
  }

  if(video_set_mode(mode)) {
    printf("Error in vg_init()\n");
    return 1;
  }

  if(video_draw_pattern(no_rectangles, first, step)) {
    printf("Error in video_draw_pattern()\n");
    return 1;
  }


  wait_for_esc();

  if(vg_exit()) {
    printf("ERror in vg_exit() \n");
    return 1;
  }

  return 0; 
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  
  if(set_mem(0x105)) {
    printf("Could not set the memory\n");
    return 1;
  }

  if (video_set_mode(0x105)){
    printf("Error in vg_init()\n");
    return 1;
  }

  if(video_draw_xpm(xpm, x, y)) {
    printf("Error in video_draw_xpm()\n");
    return 1;
  }

  wait_for_esc();

  if(vg_exit()) {
    printf("Error in vg_exit()\n");
    return 1;
  }

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  
  bool horizontal = false;

  if (set_mem(0x105)) {
    printf("Could not set the memory\n");
    return 1;
  }

  if(video_set_mode(0x105)){
    printf("Error in vg_init()\n");
    return 1;
  }

  if(timer_set_frequency(0,fr_rate)) {
    printf("Error in timer_set_frequency()\n");
    return 1;
  }

  if(xi!=xf) horizontal = true;

  uint8_t timer_irq, kbd_irq;
  int r, ipc_status, idx = 0;
  message msg;
  uint8_t bytes[2];
  extern uint8_t scancode;
  extern int counter;

  int16_t movement;
  

  if(timer_subscribe_int(&timer_irq)){
    printf("Error in timer_subscribe_int()\n");
    return 1;
  }

  if(kbd_subscribe_int(&kbd_irq)) {
    printf("Error in keyboard_subscribe_int()\n");
    return 1;
  }

  while (bytes[idx] != ESC_BRK) {

    if((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("Error in driver_receive()\n");
      continue;
    }

    if(is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {

      if (msg.m_notify.interrupts && timer_irq){

        if ((speed < 0 && counter == speed) || speed > 0) {

          video_draw_rectangle(xi,yi, 100,100,0x000000);

          movement = speed < 0 ? 1 : speed;

          if(horizontal) {
            
            if(xi + movement < xf) xi += movement;
            else {
              xi = xf;
              break;
            }
            
          }
          else {

            if (yi + movement < yf) yi += movement;
            else {
              yi = yf;
              break;
            }

          }

          counter = 0;

          video_draw_xpm(xpm, xi, yi);

        }

        else counter--;
      
      }

      if (msg.m_notify.interrupts && kbd_irq) {
        kbc_ih();
        if (scancode == TWO_BYTE) {
          bytes[idx] = scancode;
          idx++;
        }
        else {
          bytes[idx] = scancode;
          idx = 0;
        }
      }

    }

  }


  if(timer_unsubscribe_int()) {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }

  if(kbd_unsubscribe_int()) {
    printf("Error in keyboard_unsubscribe_int()\n");
    return 1;
  }

  if (vg_exit()) {
    printf("Error in vg_exit()\n");
    return 1;
  }

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}

