#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <lcom/timer.h>
#include "keyboard.h"
#include "timer.c"
#include "i8254.h"

#include <stdbool.h>
#include <stdint.h>

extern uint8_t scancode, size, status, bytes[2];
extern uint32_t count_inb_calls;
extern int hook_id, ret, counter;

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
    // subscribe keyboard interrupts
    uint8_t irq_set;
    
    if (kbd_subscribe_int(&irq_set) != 0) {
        perror("Error subscribing keyboard interrupts\n");
        return 1;
    }

    int ipc_status;
    message msg;
    uint8_t r;
    do {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & BIT(irq_set)) {
                        if (util_sys_inb(STAT_REG, &status)) return 1;
                        kbc_ih();
                        if (ret) return 1;
                        if (size == 0) {
                            bytes[size] = scancode;
                            if (scancode == TWO_BYTE_CODE) size++;
                            else kbd_print_scancode(!(scancode & MAKE_BIT), size+1, bytes);
                        }
                        else {
                            bytes[size] = scancode;
                            size = 0;
                            kbd_print_scancode(!(scancode & MAKE_BIT), size+2, bytes);
                        }
                        kbd_print_no_sysinb(count_inb_calls);
                    }
                    break;
                default: break;
            }
        }
    } while (scancode != ESC_BREAK);

    if (kbd_unsubscribe_int()) {
        perror("Error unsubscribing keyboard interrupts\n");
        return 1;
    }

    return 0;
}

int(kbd_test_poll)() {    
    do {
        if (util_sys_inb(STAT_REG, &status)) return 1;
        if (status & OBF) {
            kbc_ih();
            if (ret) return 1;
            if (size == 0) {
                bytes[size] = scancode;
                if (scancode == TWO_BYTE_CODE) size++;
                else kbd_print_scancode(!(scancode & MAKE_BIT), size+1, bytes);
            }
            else {
                bytes[size] = scancode;
                size = 0;
                kbd_print_scancode(!(scancode & MAKE_BIT), size+2, bytes);
            }
        }
        tickdelay(micros_to_ticks(DELAY_US));
    } while (scancode != ESC_BREAK);

    // enable keyboard interrupts
    if (kbd_enable_int()) {
        perror("Error enabling keyboard interrupts\n");
        return 1;
    }

    return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
    // subscribe timer interrupts
    uint8_t irq_set_timer;
    if (timer_subscribe_int(&irq_set_timer)) {
        perror("Error subscribing timer interrupts\n");
        return 1;
    }

    // set timer freq to 60
    if (timer_set_frequency(0, 60)) {
        perror("Error setting timer 0 frequency\n");
        return 1;
    }
    
    // subscribe keyboard interrupts
    uint8_t irq_set;
    
    if (kbd_subscribe_int(&irq_set) != 0) {
        perror("Error subscribing keyboard interrupts\n");
        return 1;
    }

    int ipc_status;
    message msg;
    uint8_t r;
    do {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & BIT(irq_set)) {
                        if (util_sys_inb(STAT_REG, &status)) return 1;
                        kbc_ih();
                        if (ret) return 1;
                        if (size == 0) {
                            bytes[size] = scancode;
                            if (scancode == TWO_BYTE_CODE) size++;
                            else kbd_print_scancode(!(scancode & MAKE_BIT), size+1, bytes);
                        }
                        else {
                            bytes[size] = scancode;
                            size = 0;
                            kbd_print_scancode(!(scancode & MAKE_BIT), size+2, bytes);
                        }
                        kbd_print_no_sysinb(count_inb_calls);
                    }

                    if (msg.m_notify.interrupts & BIT(irq_set_timer)) timer_int_handler();
                    break;
                default: break;
            }
        }
    } while (scancode != ESC_BREAK && counter < n*60);

    if (kbd_unsubscribe_int()) {
        perror("Error unsubscribing keyboard interrupts\n");
        return 1;
    }

    if (timer_unsubscribe_int()) {
        perror("Error unsubscribing timer interrupts\n");
        return 1;
    }
    
    return 0;
}

