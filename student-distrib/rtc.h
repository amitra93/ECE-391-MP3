/* rtc.h - RTC handler (work in progress)
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"

/* Constants */
#define REGISTER		0x70
#define IO_PORT			0x71
#define STATUS_REG_A	0x8A
#define STATUS_REG_B	0x8B
#define STATUS_REG_C	0x8C
#define BIT6_MASK		0x40
#define INT_MAX			0x7FFFFFFF
#define SIXTEEN			0x0F
#define TOP4BITS		0xF0

volatile int interrupt_received;


/* Externally-visible functions */

/*
 * int rtc_open(const uint8_t* filename);
 * DESCRIPTION: This function opens the RTC
 *
 * INPUTS: filename
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
int rtc_open(const uint8_t* filename);


/*
 * rtc_read(int32_t fd, void* buf, int32_t nbytes);
 * DESCRIPTION: This function reads from RTC Register C and processes the data.
 *
 * INPUTS: fd, buf, nbytes
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
int rtc_read(int32_t fd, void* buf, int32_t nbytes);


/*
 * int rtc_write(int32_t fd, const void* buf, int32_t nbytes);
 * DESCRIPTION: This function writes a new frequency
 *
 * INPUTS: fd, buf, nbytes
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
int rtc_write(int32_t fd, const void* buf, int32_t nbytes);


/*
 * int rtc_close(int32_t fd);
 * DESCRIPTION: This function reads from RTC Register C and processes the data.
 *
 * INPUTS: fd
 * OUTPUTS: returns 0 on success, 1 on failure
 * SIDE EFFECTS: none
 */
int rtc_close(int32_t fd);




/*
 * void rtc_process_interrupt(void)
 * DESCRIPTION: This function reads from RTC Register C and processes the data.
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: none
 */
void rtc_process_interrupt(void);


/*
 * void rtc_init(void)
 * DESCRIPTION: This function initializes the RTC
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: initializes the RTC
 */
void rtc_init(void);

/*
 * int32_t rtc_set_frequency(int32_t frequency)
 * DESCRIPTION: This function sets the frequency
 *
 * INPUTS: frequency
 * OUTPUTS: returns 0 on success, -1 on failure
 * SIDE EFFECTS: initializes the RTC
 */
int32_t rtc_set_frequency(int32_t frequency);


/*
 * int32_t rtc_get_power_of_2(int32_t number)
 * DESCRIPTION: This function returns the log base 2 of a number
 *
 * INPUTS: frequency in a power of 2
 * OUTPUTS: the power of 2
 * SIDE EFFECTS: initializes the RTC
 */
int32_t rtc_get_power_of_2(int32_t number);

#endif /* _RTC_H */
