/* rtc.c - RTC handler (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"

void
rtc_init(void)
{
	cli();
	
	// select Register B and disable NMI
	outb(STATUS_REG_B, REGISTER);
	// read current value of Register B
	char prev = inb(IO_PORT);
	// set index again because read resets register
	outb(STATUS_REG_B, REGISTER);
	// turn on bit 6 of Register B
	outb(prev | BIT6_MASK, IO_PORT);
	
	sti();	
}

void
process_rtc(void)
{
	// Register C must be read on each interrupt, else more interrupts will not be generated
	outb(STATUS_REG_C, REGISTER);
	// we don't care about the value for now
	inb(IO_PORT);

}
