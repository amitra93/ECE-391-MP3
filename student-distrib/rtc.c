/* rtc.c - RTC handler (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "sched.h"

int
rtc_open(const uint8_t* filename)
{
	rtc_init();
	enable_irq(8);
	
	//set default frequency to 2 hz
	rtc_set_frequency(2);
	return 0;
}


int
rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	interrupt_received = 0;
	while (!interrupt_received);
	return 0;
}


int
rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
	return rtc_set_frequency(nbytes);
}


int
rtc_close(int32_t fd)
{
	rtc_set_frequency(2);
	return 0;
}



void
rtc_process_interrupt(void)
{
	cli();
	// Register C must be read on each interrupt, else more interrupts will not be generated
	outb(STATUS_REG_C, REGISTER);
	// we don't care about the value for now
	inb(IO_PORT);
	interrupt_received = 1;
	sti();
}


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

	interrupt_received = 0;

	sti();

}

int32_t
rtc_set_frequency(int32_t frequency){
	if (frequency < 2 || frequency > 1024){
		return -1;
	}
	cli();
	int32_t power_of_2 = rtc_get_power_of_2(frequency);
	if (power_of_2 == INT_MAX){
		return -1;
	}
	int32_t rate = (16 - power_of_2);
	rate &= SIXTEEN;
	outb(STATUS_REG_A, REGISTER);
	char prev = inb(IO_PORT);
	outb(STATUS_REG_A, REGISTER);
	outb((prev & TOP4BITS) | rate, IO_PORT);
	sti();
	return 0;
}

int32_t
rtc_get_power_of_2(int32_t number){
	if (number == 1){
		return 0;
	}
	if (number % 2 == 1){
		return INT_MAX;
	}
	return 1 + rtc_get_power_of_2(number/2);
}
