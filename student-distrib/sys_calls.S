#define ASM 1

.data

	/* Set up the assembly interface as global */
	.globl halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

	/* Get the externally defined C handlers */
	.extern do_halt, do_halt, do_execute, do_read, do_write, do_open, do_close,
			do_getargs, do_vidmap, do_set_handler, do_sigreturn

.text

.align 4

halt:



system_call:
	pushl $
	pushl %ebx
	pushl %esi
	pushl %edi

	cmpl  $0, %eax
	jle	  done
	
	cmpl  $10, %eax
	jge   done
	
	call *system_call_table-4(, %eax, 4)
	
done:
	popf
	iret

# Create the jump table for the system calls
system_call_table:
.long do_halt, do_execute, do_read, do_write, do_open, do_close, do_getargs, do_vidmap, do_set_handler, do_sigreturn
