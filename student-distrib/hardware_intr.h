#ifndef _HARDWARE_INTR_H_
#define _HARDWARE_INTR_H_



/*___________HARDWARE INTERRUPTS___________*/

/* This function is listed in the IDT when IRQ1 is generated, and calls handles keyboard interrupts. It calls helper functions to process keypresses. */
extern void idt_keyboard(void); 

/* This function is listed in the IDT when IRQ8 is generated and handles RTC interrupts. It calls helper functions to process RTC interrupts */
extern void idt_rtc(void);		

#endif /* _HARDWARE_INTR_H_ */
