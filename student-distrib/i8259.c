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
	unsigned long flags;
	
	cli_and_save(flags);
	
	// Initialize masks to disable all interrupts
	master_mask = INTERRUPTS_MASKED;
	slave_mask = INTERRUPTS_MASKED;
	
	
	// start the initialization sequence for master
	outb(ICW1, MASTER_COMMAND);
	// vector offset for master
	outb(ICW2_MASTER, MASTER_DATA);
	// tell Master PIC there is a slave PIC at IRQ 2
	outb(ICW3_MASTER, MASTER_DATA);
	// enable 8086 mode
	outb(ICW4, MASTER_DATA);
	
	// start the initialization sequence for slave
	outb(ICW1, SLAVE_COMMAND);
	// vector offset for slave
	outb(ICW2_SLAVE, SLAVE_DATA);
	// tell slave PIC its cascade identity (2)
	outb(ICW3_SLAVE, SLAVE_DATA);
	//enable 8086 mode
	outb(ICW4, SLAVE_DATA);
	
	// restore interrupt masks
	outb(master_mask, MASTER_DATA);
	outb(slave_mask, SLAVE_DATA);
	
	restore_flags(flags);
	sti();
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	unsigned long flags;
	
	cli_and_save(flags);
	
	// create mask to later AND with
	uint8_t mask = ~ (1 << (irq_num & 7));
	
	if (irq_num & 8){
		// send mask to slave and 2 to master
		slave_mask &= mask;
		outb(slave_mask, SLAVE_DATA);
		//send 2 to master
		master_mask &= UNMASK_2;
		outb(master_mask, MASTER_DATA);
	}
	else {
		// send to master only
		master_mask &= mask;
		outb(master_mask, MASTER_DATA);
	}
	
	restore_flags(flags);
	sti();
	
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	unsigned long flags;
	
	// create mask to later OR with
	uint8_t mask = 1 << (irq_num & 7);
	cli_and_save(flags);
	
	if (irq_num & 8){
		// send to slave
		slave_mask |= mask;
		outb(slave_mask, SLAVE_DATA);
	}
	else {
		// send to master
		master_mask |= mask;
		outb(master_mask, MASTER_DATA);
	}
	
	restore_flags(flags);
	sti();
	
	
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	
	unsigned long flags;
	
	cli_and_save(flags);
	
	if (irq_num & 8){
		// send 2 to master and irq_num-8 to slave
		outb(EOI | (irq_num & 7), SLAVE_COMMAND);
		outb(EOI | CASCADE_IR, MASTER_COMMAND);
	}
	else {
		// send to master only
		outb(EOI | irq_num, MASTER_COMMAND);
	}
	
	restore_flags(flags);
}

