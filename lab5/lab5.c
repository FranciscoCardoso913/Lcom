// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8254.h"
#include "i8042.h"
#include "keyboard.h"
#include "video.h"

extern uint32_t counter_timer;
extern uint8_t size, bytes[2];

int main(int argc, char* argv[]) {
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
    int ipc_status;
    uint32_t freq = sys_hz();
    message msg;
    uint8_t irq_timer, r;

    if (init_vars(mode) != 0) {
        printf("init_vars() failed\n");
        return 1;
    }

    if (video_set_mode(mode) != 0) {
        printf("video_set_mode(%d) failed\n", mode);
        return 1;
    }

    if (timer_subscribe_int(&irq_timer) != 0) {
        printf("timer_subscribe_int() failed\n");
        return 1;
    }

    while (delay) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive() failed\n");
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:
                if (msg.m_notify.interrupts & irq_timer) {
                    timer_int_handler();
                    if (counter_timer % freq == 0) delay--;
                }
                break;
            default:
                break;
            }
        }
    }

    if (timer_unsubscribe_int() != 0) {
        printf("timer_unsubscribe_int() failed\n");
        return 1;
    }

    if (text_mode() != 0) {
        printf("text_mode() failed\n");
        return 1;
    }

    return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    int ipc_status;
    message msg;
    uint8_t irq_kbd, r;

    if (init_vars(mode) != 0) {
        printf("init_vars() failed\n");
        return 1;
    }

    if (video_mode() != 0) {
        printf("video_mode() failed\n");
        return 1;
    }

    if (keyboard_subscribe_int(&irq_kbd) != 0) {
        printf("keyboard_subscribe_int() failed\n");
        return 1;
    }

    //if (vg_draw_rectangle(x, y, width, height, color) != 0) {
    //    printf("vbe_draw_rectangle() failed\n");
    //    return 1;
    //}

    do {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != F_OK) continue;

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:
                if (msg.m_notify.interrupts & irq_kbd) {
                    keyboard_ih();
                    if (bytes[size] == TWO_BYTES) size++;
                    else size = 0;
                }
            default: break;
            }
        }
    } while (bytes[size] != ESC_BREAK);

    if (keyboard_unsubscribe_int() != 0) {
        printf("keyboard_unsubscribe_int() failed\n");
        return 1;
    }

    if (text_mode() != 0) {
        printf("vbe_text_mode() failed\n");
        return 1;
    }

    return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
    /* To be completed */
    printf("%s(0x%03x, %u, 0x%08x, %d): under construction\n", __func__,
        mode, no_rectangles, first, step);

    return 1;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    /* To be completed */
    printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

    return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
    int16_t speed, uint8_t fr_rate) {
    /* To be completed */
    printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
        __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

    return 1;
}

int(video_test_controller)() {
    /* To be completed */
    printf("%s(): under construction\n", __func__);

    return 1;
}
