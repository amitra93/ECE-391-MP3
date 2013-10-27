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

/* Externally-visible functions */

/*
 * void process_rtc(void)
 * DESCRIPTION: This function reads from RTC Register C and processes the data.
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: none
 */
void process_rtc(void);


/*
 * void rtc_init(void)
 * DESCRIPTION: This function initializes the RTC
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: initializes the RTC
 */
void rtc_init(void);



#endif /* _RTC_H */
