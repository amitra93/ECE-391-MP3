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
	
	outb(REGISTER, STATUS_REG_B);
	char prev = inb(IO_PORT);
	outb(REGISTER, STATUS_REG_B);
	outb(IO_PORT, prev | 0x40);
	
	sti();	
}

void
process_rtc(void)
{
	outb(REGISTER, STATUS_REG_C - 0x80);
	inb(IO_PORT);

}
