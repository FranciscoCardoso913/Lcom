#include <lcom/lcf.h>
#include <../lab2/i8254.h>

uint32_t counter = 0;
int hookid_timer = 1;

int (timer_subscribe_int) (uint8_t *bit_no) {
  *bit_no = hookid_timer;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hookid_timer)) {
    printf("Error when subscribing to timer\n");
    return 1;
  }
  return 0;
}

int (timer_unsubscribe_int) () {
  if (sys_irqrmpolicy(&hookid_timer)) {
    printf("Error when unsubscribing to timer\n");
    return 1;
  }
  return 0;
}

void (timer_int_handler) () {
  counter++;
}
