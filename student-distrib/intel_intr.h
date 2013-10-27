#ifndef _INTEL_INTR_H_
#define _INTEL_INTR_H_

extern void idt_unknown_intr(void); /* Unknown Interrupt */
extern void idt_intel_de(void); /* Interrupt 0 */
extern void idt_intel_db(void); /* Interrupt 1 */
extern void idt_intel_nmi(void);/* Interrupt 2 */
extern void idt_intel_bp(void); /* Interrupt 3 */
extern void idt_intel_of(void); /* Interrupt 4 */
extern void idt_intel_br(void); /* Interrupt 5 */
extern void idt_intel_ud(void); /* Interrupt 6 */
extern void idt_intel_nm(void); /* Interrupt 7 */
extern void idt_intel_df(void); /* Interrupt 8 */
extern void idt_intel_cso(void);/* Interrupt 8 */
extern void idt_intel_ts(void); /* Interrupt 10 */
extern void idt_intel_np(void); /* Interrupt 11 */
extern void idt_intel_ss(void); /* Interrupt 12 */
extern void idt_intel_gp(void); /* Interrupt 13 */
extern void idt_intel_pf(void); /* Interrupt 14 */
extern void idt_intel_mf(void); /* Interrupt 16 */
extern void idt_intel_ac(void); /* Interrupt 17 */
extern void idt_intel_mc(void); /* Interrupt 18 */
extern void idt_intel_xf(void); /* Interrupt 19 */
extern void idt_keyboard(void); /* Interrupt 33 */
extern void idt_rtc(void);		/* Interrupt 40 */

#endif /* _INTEL_INTR_H_ */
