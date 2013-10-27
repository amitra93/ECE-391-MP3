/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT 	0x20
#define SLAVE_8259_PORT  	0xA0
#define MASTER_COMMAND		MASTER_8259_PORT
#define SLAVE_COMMAND		SLAVE_8259_PORT
#define MASTER_DATA			(MASTER_COMMAND + 1)
#define SLAVE_DATA			(SLAVE_COMMAND + 1)

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1    			0x11
#define ICW2_MASTER   		0x20
#define ICW2_SLAVE    		0x28
#define ICW3_MASTER   		0x04
#define ICW3_SLAVE    		0x02
#define ICW4          		0x01

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI            		 0x60

/* Other constants */
#define CASCADE_IR			0x02
#define INTERRUPTS_MASKED	0xFF
#define UNMASK_2			0xFB

/* Externally-visible functions */

/*
 * void i8259_init(void)
 * DESCRIPTION: This function initializes the PIC
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: PIC is now capable of generating intrrupts
 */
void i8259_init(void);
/*
 * void enable_irq(uint32_t irq_num)
 * DESCRIPTION: This function enables(unmasks) the specified IRQ
 *
 * INPUTS: irq_num
 * OUTPUTS: none
 * SIDE EFFECTS: Specified IRQ is now capable of generating interrupts
 */
void enable_irq(uint32_t irq_num);
/*
 * void disable_irq(uint32_t irq_num)
 * DESCRIPTION: This function disables(masks) the specified IRQ
 *
 * INPUTS: irq_num
 * OUTPUTS: none
 * SIDE EFFECTS: Specified IRQ is now incapable of generating interrupts
 */
void disable_irq(uint32_t irq_num);

/*
 * void send_eoi(uint32_t irq_num)
 * DESCRIPTION: This function sends end-of-interrupt signal for the specified IRQ
 *
 * INPUTS: irq_num
 * OUTPUTS: none
 * SIDE EFFECTS: Specified IRQ can now send interrupts again
 */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
