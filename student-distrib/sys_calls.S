#define ASM 1

.data

	/* Set up the assembly interface as global */
	.globl halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn, system_call

	/* Get the externally defined C handlers */
	.extern do_halt, do_halt, do_execute, do_read, do_write, do_open, do_close
	.extern do_getargs, do_vidmap, do_set_handler, do_sigreturn
			
.text
.align 4
halt:
	movl $1, %eax
	jmp _system_calls
	
execute:
	movl $2, %eax
	jmp _system_call
	
read:
	movl $3, %eax
	jmp _system_call
	
write:
	movl $4, %eax
	jmp _system_call

open:
	movl $5, %eax
	jmp _system_call
	
close:
	movl $6, %eax
	jmp _system_call

getargs:
	movl $7, %eax
	jmp _system_call

vidmap:
	movl $8, %eax
	jmp _system_call
	
set_handler:
	movl $9, %eax
	jmp _system_call
	
sigreturn:
	movl $10, %eax
	jmp _system_call
	
_system_call:
	pushl %esp
	movl  %esp, %ebp

	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx

	int $0x80
	
	/* Move register return value onto stack */
	movl %eax, 20(%ebp)
	
	ret
	
system_call:
	pushl %ebx
	pushl %esi
	pushl %edi

	cmpl  $1, %eax
	jl	  done
	
	cmpl  $10, %eax
	jg   done
	
	call *system_call_table-4(, %eax, 4)
	
done:
	popl %edi
	popl %esi
	popl %ebx
	ret
	

# Create the jump table for the system calls
system_call_table:
.long do_halt, do_execute, do_read, do_write, do_open, do_close, do_getargs, do_vidmap, do_set_handler, do_sigreturn
