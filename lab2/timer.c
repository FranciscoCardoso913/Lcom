#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>


#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  uint8_t st;
  if(timer_get_conf(timer,&st)){
    printf("Error in time_get_config");
    return 1;
  };
  uint8_t word = ((BIT(3)|BIT(2)|BIT(0)|BIT(1)) & (st))| TIMER_LSB_MSB|TIMER_RB_SEL(timer);
  uint8_t f=TIMER_FREQ/freq;
  uint8_t lsb;
  uint8_t msb;
  uint8_t port;
  switch (timer)
  {
  case 0:
    port=TIMER_0;
    break;
  case 1:
    port=TIMER_1;
    break;
  case 2:
    port=TIMER_2;
    break;  
  default:
  printf("Invalid timer");
  return 1;
    break;
  }
  util_get_LSB(f,&lsb);
  util_get_MSB(f,&msb);
  if(sys_outb(TIMER_CTRL,word)){
    printf("ERROR");
    return 1;
  }
  if(sys_outb(port,lsb)){
     printf("ERROR");
    return 1;
  }
  if(sys_outb(port,msb)){
     printf("ERROR");
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
  /* To be implemented by the students */
  uint8_t word=TIMER_RB_COUNT_|TIMER_RB_CMD;
  uint8_t port;
  int error;
  switch (timer)
  {
  case 0:
    port=TIMER_0;
    word|=BIT(1);
    break;
  case 1:
    port=TIMER_1;
    word|=BIT(2);
    break;
  case 2:
    port=TIMER_2;
    word|=BIT(3);
    break;    
  
  default:
    printf("Invalid Timer");
    return 1;
    break;
  }
  error = sys_outb(TIMER_CTRL, word);
  if(error){
    printf("invalid control word");
    return error;
  }
  error= util_sys_inb(port,st);

  return error;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
 union timer_status_field_val value;
uint8_t aux;
 switch (field)
 {
  case tsf_all:
    value.byte=st;
    break;
  case tsf_initial:
    aux=st & TIMER_LSB_MSB;
    if(aux==TIMER_LSB_MSB) value.in_mode=MSB_after_LSB ;
    else if(aux==TIMER_LSB)value.in_mode=LSB_only;
    else if(aux==TIMER_MSB) value.in_mode=MSB_only;
    else value.in_mode=INVAL_val;
    break; 
  case tsf_mode:
    value.count_mode=st & (BIT(3)| BIT(2)|BIT(1)) ;
    value.count_mode>>=1;
  break;   
  case tsf_base:
    value.bcd=st & TIMER_BCD;
  break;
 default:
  printf("Invalid field!");
  break;
 }
  timer_print_config(timer,field,value);

  return 1;
}
