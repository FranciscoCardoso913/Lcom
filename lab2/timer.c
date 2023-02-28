#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id = 0;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
    // get status
    uint8_t st;
    timer_get_conf(timer, &st);

    // build control command
    st = st & 0x0f;     // don't change the 4 LS bits

    switch (timer) {    // select right timer
        case 0:
            st = st | TIMER_SEL0; break;
        case 1: 
            st = st | TIMER_SEL1; break;
        case 2: 
            st = st | TIMER_SEL2; break;
        default: break;
    }

    st = st | TIMER_LSB_MSB;    // select both bytes to write new freq

    // write control command
    if (sys_outb(TIMER_CTRL, st)) return 1;

    // write initial time to respective timer
    uint8_t lsb, msb;
    util_get_LSB((TIMER_FREQ/freq), &lsb);
    util_get_MSB((TIMER_FREQ/freq), &msb);

    if (sys_outb(TIMER_0+timer, lsb) || sys_outb(TIMER_0, msb)) return 1;

    return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    *bit_no = hook_id;
    if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)) return 1;
    return 0;
}

int (timer_unsubscribe_int)() {
    if (sys_irqrmpolicy(&hook_id)) return 1;
    return 0;
}

void (timer_int_handler)() {
    counter++;
}

int (timer_get_conf) (uint8_t timer, uint8_t *st) {
    // build readback command
    uint8_t rb = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

    // send readback command
    if (sys_outb(TIMER_CTRL, rb)) return 1;

    // read status
    if (util_sys_inb(TIMER_0 + timer, st)) return 1;

    return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
    union timer_status_field_val val;
    switch (field) {
        case tsf_all:
            val.byte = st;
            break;
        case tsf_initial:
            val.in_mode = (st & TIMER_LSB_MSB) >> 4;
            break;
        case tsf_mode:
            val.count_mode = (st & TIMER_SQR_WAVE) >> 1;
            break;
        case tsf_base:
            val.bcd = st & TIMER_BCD;
            break;
        default:
            return 1;
    }

    if (timer_print_config(timer, field, val)) return 1;

    return 0;
}
