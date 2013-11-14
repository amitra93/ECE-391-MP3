#define ASM 1

.data

	/* Set up the assembly interface as global */
	.globl idt_unknown_intr, idt_intel_de, idt_intel_db, idt_intel_nmi, idt_intel_bp, idt_intel_of, idt_intel_br
	.globl idt_intel_ud, idt_intel_nm, idt_intel_df, idt_intel_cso, idt_intel_ts, idt_intel_np, idt_intel_ss
	.globl idt_intel_gp, idt_intel_pf, idt_intel_mf, idt_intel_ac, idt_intel_mc, idt_intel_xf

	/* Get the externally defined C handlers */
	.extern do_

.extern _halt
.extern _execute
.extern _read
.extern _write
.extern _open
.extern _close
.extern _getargs
.extern _vidmap
.extern _set_handler
.extern _sigreturn

.globl system_call

.text

.align 4
system_call:
	pushf

	cmpl  $0, %eax
	jle	  done
	
	cmpl  $10, %eax
	jge   done
	
	call *system_call_table-4(, %eax, 4)
	
done:
	popf
	iret

# Create the jump table for the system calls
system_call_table:
.long _halt, _execute, _read, _write, _open, _close, _getargs, _vidmap, _set_handler, _sigreturn
