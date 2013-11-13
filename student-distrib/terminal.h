/* terminal.h - Terminal driver (work in progress)
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"


/* Constants and variables */
#define BUFFER_SIZE		128
#define NUM_COLS		80

char input[BUFFER_SIZE];
int buffer_pointer;
int old_screen_x;
int old_screen_y;
int on_new_line;

/* Externally-visible functions */

/*
 * int rtc_open(const uint8_t* filename);
 * DESCRIPTION: This function opens the RTC
 *
 * INPUTS: filename
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
int terminal_open(const uint8_t* filename);

/*
 * int rtc_open(const uint8_t* filename);
 * DESCRIPTION: This function opens the RTC
 *
 * INPUTS: filename
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
int terminal_read(int32_t fd, void* buf, int32_t nbytes);

/*
 * int rtc_open(const uint8_t* filename);
 * DESCRIPTION: This function opens the RTC
 *
 * INPUTS: filename
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
int terminal_write(int32_t fd, const void* buf, int32_t nbytes);

/*
 * int rtc_open(const uint8_t* filename);
 * DESCRIPTION: This function opens the RTC
 *
 * INPUTS: filename
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
int terminal_close(int32_t fd);

/*
 * int rtc_open(const uint8_t* filename);
 * DESCRIPTION: This function opens the RTC
 *
 * INPUTS: filename
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
void terminal_backspace();

/*
 * int rtc_open(const uint8_t* filename);
 * DESCRIPTION: This function opens the RTC
 *
 * INPUTS: filename
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
void terminal_clear();

/*
 * int rtc_open(const uint8_t* filename);
 * DESCRIPTION: This function opens the RTC
 *
 * INPUTS: filename
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
void test_terminal();

#endif /* _TERMINAL_H */
