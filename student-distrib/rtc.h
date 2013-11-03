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

int interrupt_received;


/* Externally-visible functions */

int rtc_open(const uint8_t* filename);

int rtc_read(int32_t fd, void* buf, int32_t nbytes);

int rtc_write(int32_t fd, const void* buf, int32_t nbytes);

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

int32_t rtc_set_frequency(int32_t frequency);

int32_t rtc_get_power_of_2(int32_t number);

#endif /* _RTC_H */
