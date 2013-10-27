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

/* Externally-visible functions */

/* Initialize RTC */
void rtc_init(void);

void process_rtc(void);

#endif /* _RTC_H */
