
#define ASM 1

#include "x86_desc.h"

.align 4
.data
	.globl idt_keyboard, idt_rtc, idt_pit

	.extern do_idt_keyboard, do_idt_rtc, do_idt_pit
	.extern get_cur_task, save_task, load_task

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

.align 4
idt_rtc:
	pushl (_idt_rtc)
	jmp handle_idt

.align 4
idt_pit:
	movl 	%esp, local_esp
	cmpl  	$0x10, 4(%esp)
	
	jne 	from_user_space
	subl	$8, %esp
	pushl	%eax
	movl	12(%esp), %eax
	movl	%eax, 4(%esp)
	movl	16(%esp), %eax
	movl	%eax, 8(%esp)
	movl	20(%esp), %eax
	movl	%eax, 12(%esp)
	popl	%eax
from_user_space:
	
	pushl %ebp
	movl  %esp, %ebp
	subl  $4, %esp  # 0 if task is not in a system call 1 if task is in a system call
	
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	pushl (%ebp)
	pushl local_esp
	pushl 8(%ebp)
	call get_cur_task
	pushl %eax
	
	call save_state
	addl $40, %esp
	
	call *(_idt_pit)	# Go to ISR
	
	
	subl $32, %esp
	pushl 8(%ebp)
	call get_cur_task
	pushl 	%eax 			# Current task
	
	call load_state
	movl %eax, -4(%ebp)		# Save return value
	addl $8, %esp			# Pop off task and CS
	popl local_esp
	popl %ebp
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	
	cmpl $0, (%esp)
	je to_user_space
	
to_kernel_space:
	addl 	$8, 		%esp
	pushl	%eax
	pushl	%ebx
	
	movl	local_esp,	%ebx
	
	subl 	$4, 		%ebx
	movl	16(%esp),	%eax
	movl	%eax,		(%ebx)
	
	subl 	$4, 		%ebx
	movl	12(%esp), 	%eax
	movl	%eax, 		(%ebx)
	
	subl 	$4, 		%ebx
	movl	8(%esp), 	%eax
	movl	%eax, 		(%ebx)
	movl 	%ebx, 		local_esp
	
	popl	%ebx
	popl	%eax
	addl 	$20, 		%esp
	movl	local_esp, 	%esp
	iret
	
to_user_space:
	addl $8, %esp
	iret
	
local_esp:
	.long 1
	
/*
* EDI : Handler Address
*/
.align 4
handle_idt:
	pusha
	pushf
	movl  36(%esp), %edi 	# Get the handler address	
	call *%edi				# Go to ISR
	popf
	popa
	addl $4, %esp 			# Pop off function address
	iret
	
	
