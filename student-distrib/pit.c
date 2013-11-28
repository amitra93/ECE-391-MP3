#include "pit.h"
#include "lib.h"
#include "i8259.h"

#define PIT_IRQ_NUM 0

#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_1 0x41
#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND 0x43
#define PIT_FREQUENCY 1193182
#define PIT_MAX_FREQ PIT_FREQUENCY
#define PIT_MIN_FREQ 19

#define INIT_FREQUENCY 100
#define PIT_DIVISOR (PIT_FREQUENCY/INIT_FREQUENCY)

#define SELECT_CHANNEL_0			0x00
#define SELECT_CHANNEL_1			0x40
#define SELECT_CHANNEL_2			0x80
#define SELECT_CHANNEL_READ_BACK 	0xC0

#define ACCESS_MODE_LATCH_COUNT		0x00
#define ACCESS_MODE_LOBYTE			0x10
#define ACCESS_MODE_HIBYTE			0x20
#define ACCESS_MODE_LOHIBYTE		0x30

#define OPERATING_MODE_0			0x0 //interrupt on terminal count
#define OPERATING_MODE_1			0x2	//hardware re-triggerable one-shot
#define OPERATING_MODE_2			0x4	//rate generator
#define OPERATING_MODE_3			0x6	//square wave generator
#define OPERATING_MODE_4			0x8	//software triggered strobe
#define OPERATING_MODE_5			0xA	//hardware triggered strobe

#define BINARY_MODE					0x0 //16-bit Binary
#define BCD_MODE					0x1 //Four-digit BCD

int32_t pit_init()
{	
	uint8_t port, data;
		
	cli();
	port = PIT_COMMAND;
	data = ACCESS_MODE_LOHIBYTE | OPERATING_MODE_3;
	outb (data, port);
	pit_set_frequency(100);
	pit_enable();
	sti();
	
	return 0;
}

int32_t pit_set_frequency(uint32_t frequency)
{
	uint8_t dh, dl;
	unsigned long flags;
	cli_and_save(flags);
	if (frequency > PIT_MAX_FREQ || frequency < PIT_MIN_FREQ)
	{
		restore_flags(flags);
		return -1;
	}
	dh = (PIT_FREQUENCY / frequency) >> 8;
	dl = (PIT_FREQUENCY / frequency);
	outb (dl, PIT_CHANNEL_0);
	outb (dh, PIT_CHANNEL_0);
	restore_flags(flags);
	return 0;
}

void pit_process_interrupt()
{
}

void pit_enable()
{
	enable_irq(0);
}

void pit_disable()
{
	disable_irq(0);
}
