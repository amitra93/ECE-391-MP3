
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
	movl 	%esp, 		local_esp 	# Save the esp in a local label
	cmpl  	$0x10, 		4(%esp)		
	
	/*
	*  The point of this check is to create a standard iret stack regardless of if we are from user or 
	*  kernel space. This allows us to ignore the check in c code of if we are kernel or user
	*/
	
	jne 	from_user_space			# If we are from kernel space...
	subl	$8, 		%esp		# Create two new spaces in the stack
	pushl	%eax					# Save EAX
	movl	12(%esp), 	%eax		# Move the EIP
	movl	%eax, 		4(%esp)
	movl	16(%esp), 	%eax		# Move the CS
	movl	%eax, 		8(%esp)
	movl	20(%esp), 	%eax		# Move the EFLAGS
	movl	%eax, 		12(%esp)
	popl	%eax
from_user_space:					# If we are from user space, the stack is correct
	
	pushl 	%ebp					# Create a frame pointer
	movl  	%esp, 		%ebp		
	subl  	$4, 		%esp  		# 0 if task is not in a system call 1 if task is in a system call
	
	/*
	*  This next function call takes the current registers and saves it to the task's TSS
	*/
	pushl 	%edi
	pushl 	%esi
	pushl 	%edx
	pushl 	%ecx
	pushl 	%ebx
	pushl 	%eax
	pushl 	(%ebp)
	pushl 	local_esp
	call  	get_cur_task
	pushl 	%eax
	
	call 	save_state
	addl 	$36, %esp
	
	call 	*(_idt_pit)				# Go to ISR
	
	
	/*
	*  This next function call puts the task's tss registers onto the stack
	*/
	subl 	$32, 		%esp		
	pushl 	8(%ebp)
	call 	get_cur_task
	pushl 	%eax 					# Current task
	
	call 	load_state
	movl 	%eax, -4(%ebp)			# Save return value
	addl 	$8, %esp				# Pop off task and CS
	popl 	local_esp				# Get the next ESP
	
	/*
	*  This next part pops off all values from stack into the registers
	*/
	popl 	%ebp
	popl 	%eax
	popl 	%ebx
	popl 	%ecx
	popl 	%edx
	popl 	%esi
	popl 	%edi
	
	cmpl 	$0, 	(%esp)			# If the return value from the load_state...
	je to_user_space
	
	/*
	*  If we are going to kernel space, then we are potentially going to another ESP.
	*  But, since an iret without privelage change does not have an ESP to pop off,
	*  we need to set up the return stack in the next task's stack.
	*/
	
to_kernel_space:					
	addl 	$8, 		%esp		# Pop off EBP and local variable
	pushl	%eax					# Save EAX
	pushl	%ebx					# Save EBX
	
	movl	local_esp,	%ebx		# Move the next task's stack into EBX
	
	subl 	$4, 		%ebx		# Push EFLAGS
	movl	16(%esp),	%eax
	movl	%eax,		(%ebx)
	
	subl 	$4, 		%ebx		# Push CS
	movl	12(%esp), 	%eax
	movl	%eax, 		(%ebx)
	
	subl 	$4, 		%ebx		# Push EIP
	movl	8(%esp), 	%eax		
	movl	%eax, 		(%ebx)
	movl 	%ebx, 		local_esp	# Move the new stack pointer into local_esp (we still need our stack)
	
	popl	%ebx
	popl	%eax
	addl 	$20, 		%esp		# Break down the iret block in our stack
	movl	local_esp, 	%esp		# Move to the next process stack
	iret							# Context Switch

to_user_space: 		# If we are going to user space then the stack is set up for a context switch
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
	
	
