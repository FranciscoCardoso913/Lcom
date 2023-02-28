#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  uint16_t counter = TIMER_FREQ/freq;
  uint8_t st, lsb, msb, port; 

  if(timer_get_conf(timer, &st)) {
    printf("Error reading the configuration");
    return 1;
  }

  uint8_t mask = (BIT(3)|BIT(2)|BIT(1)|BIT(0));

  uint8_t ctrl = (st & mask) | TIMER_LSB_MSB;

  switch (timer) {
    case 0:
      port = TIMER_0; break;
    case 1:
      ctrl |= TIMER_SEL1;
      port = TIMER_1; break;

    case 2:
      ctrl |= TIMER_SEL2;
      port = TIMER_2; break;
  }

  if (sys_outb(TIMER_CTRL, ctrl) != OK) {
    printf("Error writing to the timer control register! \n");
    return 1;
  }

  if(util_get_LSB(counter, &lsb)){
    printf("Error getting the LSB! \n");
    return 1;
  }

  if(util_get_MSB(counter, &msb)){
    printf("Error getting the MSB! \n");
    return 1;
  }

  if (sys_outb(port, lsb) != OK) {
    printf("Error writing to the timer 0 register! \n");
    return 1;
  }
  if (sys_outb(port, msb) != OK) {
    printf("Error writing to the timer 0 register! \n");
    return 1;
  }

  

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  
  uint8_t read_back_command = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

   
  if (sys_outb(TIMER_CTRL, read_back_command) != OK) {
    printf("Error writing to the timer control register! \n");
    return 1;
  }

  switch (timer) {
    case 0:
      return util_sys_inb(TIMER_0 + timer, st); 

    case 1:
      return util_sys_inb(TIMER_1 + timer, st);

    case 2:
      return util_sys_inb(TIMER_2 + timer, st);
      
  }

  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val value;
  uint8_t mask = (BIT(3) | BIT(2) | BIT(1)); 

  switch (field) {
    case tsf_all:
      value.byte = st;
      break;
    case tsf_initial:
      value.in_mode = (st & TIMER_LSB_MSB) >> 4;
      break;
    case tsf_mode:
      value.count_mode = (st & mask) >> 1; 
      break;
    case tsf_base:
      value.bcd = st & TIMER_BCD;
      break;
  }

  return timer_print_config(timer, field, value);

}
