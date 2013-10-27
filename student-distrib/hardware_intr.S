
#define ASM 1

#include "x86_desc.h"

.align 4
.data
	.globl idt_keyboard, idt_rtc

	.extern do_idt_keyboard, do_idt_rtc


_idt_keyboard:
	.long	do_idt_keyboard

_idt_rtc:
	.long	do_idt_rtc
	
	/* 
	* The follwoing code sets up a small calling convention for error codes
	* First push the error code onto the stack (processor does this) or
	* push a null value for the error code if the processor does not (see IA manual)
	* Next, push the interrupt handler address onto the stack then jump to the error code
	*/

.text
	
.align 4

idt_keyboard:
	pushl $0
	pushl (_idt_keyboard)
	jmp error_code

idt_rtc:
	pushl $0
	pushl (_idt_rtc)
	jmp error_code

/*
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
	# movw  $KERNEL_DS, %cx	# Move the Kernel Data Segment 
	# movw  %cx, %ds 			# Move the data segment selector into ds and es
	# movw  %cx, %es	
	call *%edi				# Go to ISR
	popa					# Pop off all saved registers
	addl $8, %esp 			# Pop off error code and function address
	iret
	
	
