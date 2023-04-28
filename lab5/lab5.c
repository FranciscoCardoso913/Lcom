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

void wait_esc() {
    int ipc_status;
    message msg;
    uint8_t irq_kbd, r;

    if (keyboard_subscribe_int(&irq_kbd) != 0) {
        printf("keyboard_subscribe_int() failed\n");
    }

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
    }
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    if (video_set_mode(mode) != 0) {
        printf("video_set_mode(%d) failed\n", mode);
        return 1;
    }

    sleep(delay);

    if (vg_exit() != 0) {
        printf("text_mode() failed\n");
        return 1;
    }

    return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    uint8_t irq_kbd;

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

    if (video_draw_rectangle(x, y, width, height, color) != 0) {
        printf("video_draw_rectangle() failed\n");
    }

    wait_esc();

    if (keyboard_unsubscribe_int() != 0) {
        printf("keyboard_unsubscribe_int() failed\n");
        return 1;
    }

    if (vg_exit() != 0) {
        printf("vbe_text_mode() failed\n");
        return 1;
    }

    return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
    if (init_vars(mode) != 0) {
        printf("init_vars() failed\n");
        return 1;
    }

    if (video_set_mode(mode) != 0) {
        printf("video_mode() failed\n");
        return 1;
    }

    if (video_draw_pattern(no_rectangles, first, step) != 0) {
        printf("video_draw_pattern() failed\n");
    }

    wait_esc();

    if (vg_exit() != 0) {
        printf("vbe_text_mode() failed\n");
        return 1;
    }

    return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    if (init_vars(VBE_VIDEO_MODE) != 0) {
        printf("init_vars() failed\n");
        return 1;
    }

    if (video_set_mode(VBE_VIDEO_MODE) != 0) {
        printf("video_mode() failed\n");
        return 1;
    }

    if (print_xpm(xpm, x, y) != 0) {
        printf("video_draw_pattern() failed\n");
    }

    wait_esc();

    if (vg_exit() != 0) {
        printf("vbe_text_mode() failed\n");
        return 1;
    }

    return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
    int16_t speed, uint8_t fr_rate) {
    int ipc_status;
    message msg;
    uint8_t irq_set_TIMER, irq_set_KBC; // máscaras para as interrupções

    // o movimento só pode ser horizontal ou vertical
    uint8_t vertical_direction;
    if (xi == xf && yi < yf) vertical_direction = 1;
    else if (yi == yf && xi < xf) vertical_direction = 0;
    else return 1;

    // Subscrição das interrupções
    if (keyboard_subscribe_int(&irq_set_KBC) != 0) return 1;
    if (timer_subscribe_int(&irq_set_TIMER) != 0) return 1;

    // Atualiza o sistema para a frame rate dada
    if (timer_set_frequency(0, fr_rate) != 0) return 1;

    // Construção do frame buffer virtual e físico (só permite modo indexado)
    if (init_vars(VBE_VIDEO_MODE)) return 1;

    // Mudança para o modo gráfico (só permite modo indexado)
    if (video_set_mode(VBE_VIDEO_MODE)) return 1;

    // Imprime a imagem na posição inicial
    if (print_xpm(xpm, xi, yi) != 0) return 1;

    bool done = false;

    do {

        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            printf("Error");
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:

                // interrupção do teclado
                if (msg.m_notify.interrupts & irq_set_KBC) {
                    keyboard_ih(); // atualiza o scancode
                    if (bytes[size] == TWO_BYTES) size++;
                    else size = 0;
                }

                // interrupção do timer
                if (msg.m_notify.interrupts & irq_set_TIMER) {

                    // Apaga o desenho anterior para dar a sensação de movimetno
                    if (video_draw_rectangle(xi, yi, 100, 100, 0xFFFFFF) != 0) return 1;

                    // Atualiza o valor da variável a incrementar
                    if (vertical_direction) {
                        yi += speed;
                        if (yi >= yf) {
                            done = true;
                            break;
                        }
                    }
                    else {
                        xi += speed;
                        if (xi >= xf) {
                            done = true;
                            break;
                        }
                    }

                    // Imprime a nova imagem ligeiramente deslocada
                    if (print_xpm(xpm, xi, yi) != 0) return 1;
                }
            }
        }
    } while (bytes[size] != ESC_BREAK && !done);


    // Desativar as interrupções
    if (timer_unsubscribe_int() != 0) return 1;
    if (keyboard_unsubscribe_int() != 0) return 1;

    // De regresso ao modo texto
    if (vg_exit() != 0) return 1;

    return 0;
}

int(video_test_controller)() {
    /* To be completed */
    printf("%s(): under construction\n", __func__);

    return 1;
}

