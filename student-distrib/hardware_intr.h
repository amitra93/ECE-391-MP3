#ifndef _HARDWARE_INTR_H_
#define _HARDWARE_INTR_H_

extern void idt_keyboard(void); /* Interrupt 33 */
extern void idt_rtc(void);		/* Interrupt 40 */

#endif /* _HARDWARE_INTR_H_ */
