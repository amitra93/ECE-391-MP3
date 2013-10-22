#define ASM 1

.text

.extern _halt
.extern _execute
.extern _read
.extern _write
.extern _open
.extern _close
.extern _getargs
.extern _vidmap
.extern _set_handler
.extern _sigreturn

.globl system_call

.align 4
system_call:
	pushf

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
.long _halt, _execute, _read, _write, _open, _close, _getargs, _vidmap, _set_handler, _sigreturn
