/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
	// http://www.brokenthorn.com/Resources/OSDevPic.html
	/*
	unsigned long flags;
	
	//cli_and_save(flags);
	
	// Mask all interrupts on the PIC
	outb(0xff, MASTER_8259_PORT);
	outb(0xff, SLAVE_8259_PORT);
	
	// Send ICW1
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);
	
	// Send ICW2
	outb(ICW2_MASTER, MASTER_8259_PORT);
	outb(ICW2_SLAVE, SLAVE_8259_PORT);
	
	// Send ICW3
	outb(ICW3_MASTER, MASTER_8259_PORT);
	outb(ICW3_SLAVE, SLAVE_8259_PORT);
	
	// Send ICW4
	outb(ICW4, MASTER_8259_PORT);
	outb(ICW4, SLAVE_8259_PORT);
	
	// restore interrupt masks
	outb(master_mask, MASTER_8259_PORT);
	outb(slave_mask, SLAVE_8259_PORT);
	
	//sti();
	//restore_flags(flags);
	*/
	
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	/*
	unsigned long flags;
	
	cli_and_save(flags);
	
	if (irq_num & 8){
		// send 2 to master and irq_num-8 to slave
		outb(EOI | (irq_num & 7), SLAVE_8259_PORT);
		outb(EOI | CASCADE_IR, MASTER_8259_PORT);
	}
	else {
		// send to master only
		outb(EOI | irq_num, MASTER_8259_PORT);
	}
	
	sti();
	restore_flags(flags);
	*/
}

