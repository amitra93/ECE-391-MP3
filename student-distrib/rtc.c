/* rtc.c - RTC handler (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"

/* Initialize the RTC */
void
rtc_init(void)
{
	cli();
	
	outb(STATUS_REG_B, REGISTER);
	char prev = inb(IO_PORT);
	outb(STATUS_REG_B, REGISTER);
	outb(prev | 0x40, IO_PORT);
	
	sti();	
}

void
process_rtc(void)
{
	outb(STATUS_REG_C - 0x80, REGISTER);
	inb(IO_PORT);

}
