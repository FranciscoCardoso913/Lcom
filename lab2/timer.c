#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

extern int counter;
extern int hookid;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  uint16_t initCount = TIMER_FREQ/freq;
  uint8_t ctrlWord = 0x0F, status, port, lsb, msb;
  util_get_LSB(initCount, &lsb);
  util_get_MSB(initCount, &msb);


  if (timer_get_conf(timer, &status)) {
    fprintf(stderr, "Error in timer_get_conf\n");
    return 1;
  }

  ctrlWord &= status;
  ctrlWord |= TIMER_LSB_MSB;

  switch(timer) {
    case 0:
      port = TIMER_0;
      break;
    case 1:
      port = TIMER_1;
      ctrlWord |= BIT(6);
      break;
    case 2:
      port = TIMER_2;
      ctrlWord |= BIT(7);
      break;
    default:
      fprintf(stderr, "Wrong value of timer\n");
      return 1;
  }
  
  if (sys_outb(TIMER_CTRL, ctrlWord)) {
    fprintf(stderr, "Error in sys_outb\n");
    return 1;
  }

  if (sys_outb(port, lsb)) {
    fprintf(stderr, "Error in util_sys_inb\n");
    return 1;
  }

  if (sys_outb(port, msb)) {
    fprintf(stderr, "Error in util_sys_inb\n");
    return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hookid;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hookid)) {
    fprintf(stderr, "Error in sys_irqsetpolicy\n");
    return 1;
  };

  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hookid)) {
    fprintf(stderr, "Error in sys_irqrmpolicy\n");
    return 1;
  }

  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t ctrlWord = TIMER_RB_CMD | TIMER_RB_COUNT_;
  uint8_t port;
  int err;

  switch(timer) {
    case 0:
      ctrlWord |= BIT(1);
      port = TIMER_0;
      break;
    case 1:
      ctrlWord |= BIT(2);
      port = TIMER_1;
      break;
    case 2:
      ctrlWord |= BIT(3);
      port = TIMER_2;
      break;
    default:
      fprintf(stderr, "Wrong timer input (0-2): %d\n", timer);
      return 1;
  }

  err = sys_outb(TIMER_CTRL, ctrlWord);

  if (err) {
    fprintf(stderr, "Error when writing control word\n");
    return err;
  }

  err = util_sys_inb(port, st);

  return err;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val val;
  
  switch(field) {
    case tsf_all:
      val.byte = st;
      break;
    case tsf_initial:
      st &= TIMER_LSB_MSB;
      if (st == TIMER_LSB_MSB) {
        val.in_mode = MSB_after_LSB;
      }
      else if (st == TIMER_LSB) {
        val.in_mode = LSB_only;
      }
      else if (st == TIMER_MSB) {
        val.in_mode = MSB_only;
      }
      else {
        val.in_mode = INVAL_val; // Retonar erro?
      }
      break;
    case tsf_mode:
        st &= (BIT(3) | BIT(2) | BIT(1));
        st >>= 1;
        if (st == 6) val.count_mode = 2;        // 2 maps to 2 different binary values
        else if (st == 7) val.count_mode = 3;   // 3 maps to 2 different binary values
        else val.count_mode = st;
      break;
    case tsf_base:
      val.bcd = st & BIT(0);
      break;
    default:
      fprintf(stderr, "Invalid timer_status_field\n");
      return 1;
  }

  if (timer_print_config(timer, field, val)) {
    fprintf(stderr, "Error in timer_print_config\n");
    return 1;
  }

  return 0;
}
