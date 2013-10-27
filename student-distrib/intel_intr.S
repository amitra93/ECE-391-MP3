
#define ASM 1

#include "x86_desc.h"

.align 4
.data

	/* Set up the assembly interface as global */
	.globl idt_unknown_intr, idt_intel_de, idt_intel_db, idt_intel_nmi, idt_intel_bp, idt_intel_of, idt_intel_br
	.globl idt_intel_ud, idt_intel_nm, idt_intel_df, idt_intel_cso, idt_intel_ts, idt_intel_np, idt_intel_ss
	.globl idt_intel_gp, idt_intel_pf, idt_intel_mf, idt_intel_ac, idt_intel_mc, idt_intel_xf

	/* Get the externally defined C handlers */
	.extern do_idt_unknown_intr, do_idt_intel_de, do_idt_intel_db, do_idt_intel_nmi, do_idt_intel_bp, do_idt_intel_of
	.extern	do_idt_intel_br, do_idt_intel_ud, do_idt_intel_nm, do_idt_intel_df, do_idt_intel_cso, do_idt_intel_ts
	.extern	do_idt_intel_np, do_idt_intel_ss, do_idt_intel_gp, do_idt_intel_pf, do_idt_intel_mf, do_idt_intel_ac
	.extern do_idt_intel_mc, do_idt_intel_xf

/* 
*  Put the addresses in memory, for some reason this was the only way the assembler
*  could recognize the functions.
*/
_idt_unknown_intr:
	.long	do_idt_unknown_intr

_idt_intel_de:
	.long	do_idt_intel_de

_idt_intel_db:
	.long	do_idt_intel_db

_idt_intel_nmi:
	.long	do_idt_intel_nmi

_idt_intel_bp:
	.long	do_idt_intel_bp

_idt_intel_of:
	.long	do_idt_intel_of

_idt_intel_br:
	.long	do_idt_intel_br

_idt_intel_ud:
	.long	do_idt_intel_ud

_idt_intel_nm:
	.long	do_idt_intel_nm

_idt_intel_df:
	.long	do_idt_intel_df

_idt_intel_cso:
	.long	do_idt_intel_cso

_idt_intel_ts:
	.long	do_idt_intel_ts

_idt_intel_np:
	.long	do_idt_intel_np

_idt_intel_ss:
	.long	do_idt_intel_ss

_idt_intel_gp:
	.long	do_idt_intel_gp

_idt_intel_pf:
	.long	do_idt_intel_pf

_idt_intel_mf:
	.long	do_idt_intel_mf

_idt_intel_ac:
	.long	do_idt_intel_ac

_idt_intel_mc:
	.long	do_idt_intel_mc

_idt_intel_xf:
	.long	do_idt_intel_xf

	
/* 
* The follwoing code sets up a small calling convention for error codes
* First push the error code onto the stack (processor does this) or
* push a null value for the error code if the processor does not (see IA manual)
* Next, push the interrupt handler address onto the stack then jump to the error code
*/

.text
	
.align 4
idt_unknown_intr:
	iret
	
idt_intel_de:
	pushl $0
	pushl (_idt_intel_de)
	jmp error_code
	
idt_intel_db:
	pushl $0
	pushl (_idt_intel_db)
	jmp error_code

idt_intel_nmi:
	pushl $0
	pushl (_idt_intel_nmi)
	jmp error_code
	
idt_intel_bp:
	pushl $0
	pushl (_idt_intel_bp)
	jmp error_code
	
idt_intel_of:
	pushl $0
	pushl (_idt_intel_of)
	jmp error_code
	
idt_intel_br:
	pushl $0
	pushl (_idt_intel_br)
	jmp error_code
	
idt_intel_ud:
	pushl $0
	pushl (_idt_intel_ud)
	jmp error_code
	
idt_intel_nm:
	pushl $0
	pushl (_idt_intel_nm)
	jmp error_code # This should have its own error_code segment
	
idt_intel_df:
	pushl (_idt_intel_df)
	jmp error_code
	
idt_intel_cso:
	pushl $0
	pushl (_idt_intel_cso)
	jmp error_code
	
idt_intel_ts:
	pushl (_idt_intel_ts)
	jmp error_code
	
idt_intel_np:
	pushl (_idt_intel_np)
	jmp error_code
	
idt_intel_ss:	
	pushl (_idt_intel_ss)
	jmp error_code
	
idt_intel_gp:
	pushl (_idt_intel_gp)
	jmp error_code
	
idt_intel_pf:
	pushl (_idt_intel_pf)
	jmp error_code
	
idt_intel_mf:
	pushl (_idt_intel_mf)
	jmp error_code
	
idt_intel_ac:
	pushl (_idt_intel_ac)
	jmp error_code
	
idt_intel_mc:
	pushl $0
	pushl (_idt_intel_mc)
	jmp error_code
	
idt_intel_xf:
	pushl $0
	pushl (_idt_intel_xf)
	jmp error_code


/*
* Lastly, we want to set up the function arguments for the C interrupt handler and return to the IP
*
* EAX : Error Code
* EBX : Instruction Pointer
* ECX : Reserved for future use of process ID
* EDI : Handler Address
*/
error_code:
	pusha 					# Push all registers to be saved, 8 Pushes
	# movl  $0, %ebx 		# In the future, place process id here in EBX. See p150 in Understanding the Linux Kernel
	movl 40(%esp), %ebx		# Get the errored instruction pointer
	movl  36(%esp), %eax  	# Get the error code
	movl  $-1, 36(%esp)		# Set the error code to -1, differentiates interrupts and sys calls
	movl  32(%esp), %edi 	# Get the handler address
	call *%edi				# Go to ISR
	popa					# Pop off all saved registers
	addl $8, %esp 			# Pop off error code and function address
	iret					# Return to code
	
	
