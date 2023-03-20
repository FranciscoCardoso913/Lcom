#include <lcom/lcf.h>
#include "i8042.h"
#include "mouse.h"

extern uint8_t byte;
extern int hookid;
int err = 0;

int read_status(uint8_t *res) {
	return util_sys_inb(STAT_REG, res);
}

int read_outbuffer(uint8_t *res) {
	int tries = 10;
	uint8_t status;

	while (tries--) {
		if (read_status(&status)) {
			fprintf(stderr, "Error while reading status\n");
			continue;
		}

		if (status & KBC_ST_OBF){
			if (util_sys_inb(OUT_BUF, res)) {
				fprintf(stderr, "Error while reading output buffer\n");
				continue;
			}

			if (status & (KBC_PAR_ERROR | KBC_TO_ERROR) || !(status & KBC_ST_AUX)) {
        fprintf(stderr, "Parity, timeout or data not comingo from mouse error\n");
				return 1;
			}
			return 0;
		}

		tickdelay(DELAY);
	}
  fprintf(stderr, "timeout\n");
	return 1;
}

/**
 * Assumes that data reporting is disabled...
*/
int issue_mouse_command(uint8_t *res, uint8_t argv[], int argc) {
	uint8_t status, arg_error;
	bool no_err;

	while (true) {
		no_err = 1;
		for (int i = 0 ; i < argc; i++) {

			if (read_status(&status)) {
				fprintf(stderr, "Error while trying to read status\n");
				no_err = 0;
				break;
			}

			if (!(status & KBC_ST_IBF)) {

				sys_outb(IN_BUF, WRITE_BYTE_MOUSE); // Write byte command

				tickdelay(DELAY);

				sys_outb(ARG_REG, argv[i]);			// Write ith argument
				
				if (read_outbuffer(&arg_error)) {
					no_err = 0;
					break;
				}

				tickdelay(DELAY);


				if (arg_error == NACK || arg_error == ERROR) {
					no_err = 0;
					break;
				}
			}

			else {
        fprintf(stderr, "IBF full\n");
				no_err = 0;
				break;
			}
		}
		if (no_err)
			break;
	}

	return 0;
}

int subscribe_mouse(uint8_t *irqset) {
	*irqset = hookid;
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookid)) {
		fprintf(stderr, "Error while subscribing to mouse events\n");
		return 1;
	}
	return 0;
}

int unsubscribe_mouse() {
	if (sys_irqrmpolicy(&hookid)) {
		fprintf(stderr, "Error while unsubscribing to mouse events\n");
		return 1;
	}
	return 0;
}

void (mouse_ih)() {
	uint8_t status;
	if (read_status(&status)) {
		fprintf(stderr, "Error while reading status\n");
		err = 1;
		return;
	}

	if (status & KBC_ST_OBF) {
		if (util_sys_inb(OUT_BUF, &byte)) {
			fprintf(stderr, "Error while reading from output buffer\n");
			err = 1;
			return;
		}

		if (status & (KBC_PAR_ERROR | KBC_TO_ERROR) || !(status & KBC_ST_AUX)) {
			fprintf(stderr, "Parity, timeout or data not coming from mouse\n");
			err = 2;
			return;
		}
	}

	err = 0;
}
