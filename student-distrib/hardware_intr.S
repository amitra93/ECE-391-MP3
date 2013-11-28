
#define ASM 1

#include "x86_desc.h"

.align 4
.data
	.globl idt_keyboard, idt_rtc, idt_pit

	.extern do_idt_keyboard, do_idt_rtc, do_idt_pit


_idt_keyboard:
	.long	do_idt_keyboard

_idt_rtc:
	.long	do_idt_rtc
	
_idt_pit:
	.long 	do_idt_pit
	
	/* 
	* The follwoing code sets up a small calling convention for hardware interrupts by pushing the interrupt handler address onto the stack and jumping to the handle_idt function
	*/

.text
	
.align 4

idt_keyboard:
	pushl (_idt_keyboard)
	jmp handle_idt

idt_rtc:
	pushl (_idt_rtc)
	jmp handle_idt

idt_pit:
	pushl (_idt_pit)
	jmp handle_idt
/*
* EDI : Handler Address
*/
handle_idt:
	pusha 					# Push all registers to be saved, 8 Pushes
	movl  32(%esp), %edi 	# Get the handler address	
	call *%edi				# Go to ISR
	popa					# Pop off all saved registers
	addl $4, %esp 			# Pop off error code and function address
	iret
	
	
