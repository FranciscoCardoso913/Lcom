#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_timer = 0;
unsigned int counter_timer = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
    uint8_t status;
    
    if (timer_get_conf(timer, &status) != F_OK) return 1;

    status &= 0x0f;
    status |= TIMER_LSB_MSB;

    if (timer == 1) status |= TIMER_SEL1;
    if (timer == 2) status |= TIMER_SEL2;

    if (sys_outb(TIMER_CTRL, status) != F_OK) return 1;
    
    uint16_t f = TIMER_FREQ / freq;
    uint8_t lsb, msb;

    util_get_LSB(f, &lsb);
    util_get_MSB(f, &msb);

    if (sys_outb(TIMER_0 + timer, lsb) != F_OK || sys_outb(TIMER_0 + timer, msb) != F_OK) return 1;

    return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    *bit_no = BIT(hook_timer);

    return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_timer);
}

int (timer_unsubscribe_int)() {
    return sys_irqrmpolicy(&hook_timer);
}

void (timer_int_handler)() {
    counter_timer++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
    uint8_t rb = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

    if (sys_outb(TIMER_CTRL, rb) != F_OK) return 1;
    if (util_sys_inb(TIMER_0 + timer, st) != F_OK) return 1;

    return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
    union timer_status_field_val val;

    switch (field) {
        case tsf_all:
            val.byte = st; break;
        case tsf_initial:
            val.in_mode = (st & TIMER_COUNTER_INIT) >> 4; break;
        case tsf_mode:
            val.count_mode = (st & TIMER_SQR_WAVE) >> 1; break;
        case tsf_base:
            val.bcd = st & 1;
        default: break;   
    }

    if (timer_print_config(timer, field, val) != F_OK) return 1;

    return 0;
}
