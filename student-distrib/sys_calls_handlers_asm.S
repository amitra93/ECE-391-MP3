#define ASM 1

.data
	.globl return_from_halt
	.globl restart_shell

.text
.align 4

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

restart_shell:
	addl	$4, %esp
	iret