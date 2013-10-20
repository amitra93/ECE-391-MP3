/*
* Interrupt handlers for Intel defines: http://courses.engr.illinois.edu/ece391/references/IA32-ref-manual-vol-3.pdf
* See section 5-23
*/

void _idt_intel_de(void){ } /* Interrupt 0 */
void _idt_intel_db(void){ } /* Interrupt 1 */
void _idt_intel_bp(void){ } /* Interrupt 3 */
void _idt_intel_of(void){ } /* Interrupt 4 */
void _idt_intel_br(void){ } /* Interrupt 5 */
void _idt_intel_ud(void){ } /* Interrupt 6 */
void _idt_intel_nm(void){ } /* Interrupt 7 */
void _idt_intel_df(void){ }  /* Interrupt 8 */
void _idt_intel_ts(void){ } /* Interrupt 10 */
void _idt_intel_np(void){ } /* Interrupt 11 */
void _idt_intel_ss(void){ } /* Interrupt 12 */
void _idt_intel_gp(void){ } /* Interrupt 13 */
void _idt_intel_pf(void){ } /* Interrupt 14 */
void _idt_intel_mf(void){ } /* Interrupt 16 */
void _idt_intel_ac(void){ } /* Interrupt 17 */
void _idt_intel_mc(void){ } /* Interrupt 18 */
void _idt_intel_xf(void){ } /* Interrupt 19 */
