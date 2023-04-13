#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "keyboard.h"

#include <stdbool.h>
#include <stdint.h>

extern uint8_t size, bytes[2];
extern unsigned int counter_timer, sys_inb_calls;

int main(int argc, char *argv[]) {
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/lab3/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

int(kbd_test_scan)() {
    uint8_t irq_set_k;
    if (keyboard_subscribe_int(&irq_set_k) != F_OK) return 1;

    int ipc_status;
    message msg;
    uint8_t r;

    do {
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != F_OK) continue;

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & irq_set_k) {
                        keyboard_ih();
                        if (bytes[size] == TWO_BYTES) size++;
                        else {
                            bool make = !(bytes[size] & MAKE_OR_BREAK);
                            kbd_print_scancode(make, ++size, bytes);
                            size = 0;
                        }
                    }
                default: break;
            }
        }
    } while (bytes[size] != ESC_BREAK);

    return keyboard_unsubscribe_int();
}

int(kbd_test_poll)() {
    uint8_t st;

    do {
        keyboard_get_status(&st);
        if (st & KBC_ERROR) {
            tickdelay(WAIT_KBC);
            continue;
        }
        if (st & KBC_OUT_FULL) {
            keyboard_read_data();

            if (bytes[size] == TWO_BYTES) size++;
            else {
                bool make = !(bytes[size] & MAKE_OR_BREAK);
                kbd_print_scancode(make, ++size, bytes);
                size = 0;
            }
        }
    } while (bytes[size] != ESC_BREAK);

    return keyboard_restore_int();
}

int(kbd_test_timed_scan)(uint8_t n) {
    uint8_t irq_set_k, irq_set_t;

    if (keyboard_subscribe_int(&irq_set_k) != F_OK) return 1;
    if (timer_subscribe_int(&irq_set_t) != F_OK) return 1;
    //unsigned int freq = sys_hz();

    int ipc_status;
    message msg;
    uint8_t r;

    do {
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != F_OK) continue;

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & irq_set_t) timer_int_handler();
                    if (msg.m_notify.interrupts & irq_set_k) {
                        keyboard_ih();
                        if (bytes[size] == TWO_BYTES) size++;
                        else {
                            bool make = !(bytes[size] & MAKE_OR_BREAK);
                            kbd_print_scancode(make, ++size, bytes);
                            size = 0;
                        }
                        counter_timer = 0;
                    }
                default: break;
            }
        }
    } while (bytes[size] != ESC_BREAK && counter_timer < 60 * n);

    if (keyboard_unsubscribe_int() != F_OK) return 1;
    if (timer_unsubscribe_int() != F_OK) return 1;
    kbd_print_no_sysinb(sys_inb_calls);

    return 0;
}
