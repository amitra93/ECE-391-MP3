#define ASM 1

.data
	.globl return_from_halt
	.globl restart_shell

.text
.align 4

# If the task to halt isn't a shell then we go here to set up the return stack
return_from_halt:
	pushl %ebp
	movl  %esp, %ebp
	
	movb 8(%ebp), %al
	movl 12(%ebp), %ebx
	movl 16(%ebp), %ecx
	movl 20(%ebp), %edx
	
	movl %ebx, %ebp
	movl %ecx, %esp
	
	jmp *%edx

# This function allows for the semantics of a C-Function call, without having to write inline assembly
restart_shell:
	addl	$4, %esp
	iret
