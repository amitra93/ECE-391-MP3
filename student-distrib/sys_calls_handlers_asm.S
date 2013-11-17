#define ASM 1

.data
	.globl return_from_halt

.text
.align 4

return_from_halt:
	pushl %ebp
	movl  %esp, %ebp
	
	movb 8(%ebp), %ax
	movl 12(%ebp), %ebx
	movl 16(%ebp), %ecx
	movl 20(%ebp), %edx
	
	movl %ebx, %ebp
	movl %ecx, %esp
	
	jmp *%edx