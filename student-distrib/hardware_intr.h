#ifndef _HARDWARE_INTR_H_
#define _HARDWARE_INTR_H_



/*___________HARDWARE INTERRUPTS___________*/

/*
 * extern void idt_keyboard(void)
 * DESCRIPTION: This function is listed in the IDT when IRQ1 is generated, and calls handles keyboard interrupts. It calls helper functions to process keypresses.
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: Sends relevant keypresses to the console.
 */
extern void idt_keyboard(void); 

/*
 * extern void idt_rtc(void)
 * DESCRIPTION: This function is listed in the IDT when IRQ8 is generated and handles RTC interrupts. It calls helper functions to process RTC interrupts.
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: Calls RTC interrupt testing functions
 */
extern void idt_rtc(void);		

#endif /* _HARDWARE_INTR_H_ */
