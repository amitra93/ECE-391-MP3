
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
	pushl (_idt_keyboard)
	jmp handle_idt

idt_rtc:
	pushl (_idt_rtc)
	jmp handle_idt

/*
* EAX : Error Code
* EBX : Instruction Pointer
* ECX : Reserved for future use of process ID
* EDI : Handler Address
*/
handle_idt:
	pusha 					# Push all registers to be saved, 8 Pushes
	movl  32(%esp), %edi 	# Get the handler address	
	call *%edi				# Go to ISR
	popa					# Pop off all saved registers
	addl $4, %esp 			# Pop off error code and function address
	iret
	
	
