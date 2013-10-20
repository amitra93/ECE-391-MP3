
#define ASM 1

.text
	.globl idt_intel_de
	.globl idt_intel_db
	.globl idt_intel_bp
	.globl idt_intel_of
	.globl idt_intel_br
	.globl idt_intel_ud
	.globl idt_intel_nm
	.globl idt_intel_df
	.globl idt_intel_ts
	.globl idt_intel_np
	.globl idt_intel_ss
	.globl idt_intel_gp
	.globl idt_intel_pf
	.globl idt_intel_mf
	.globl idt_intel_ac
	.globl idt_intel_mc
	.globl idt_intel_xf

	.extern _idt_intel_de
	.extern _idt_intel_db
	.extern _idt_intel_bp
	.extern _idt_intel_of
	.extern _idt_intel_br
	.extern _idt_intel_ud
	.extern _idt_intel_nm
	.extern _idt_intel_df
	.extern _idt_intel_ts
	.extern _idt_intel_np
	.extern _idt_intel_ss
	.extern _idt_intel_gp
	.extern _idt_intel_pf
	.extern _idt_intel_mf
	.extern _idt_intel_ac
	.extern _idt_intel_mc
	.extern _idt_intel_xf


idt_intel_de:
	pusha # push all registers
	call  _idt_intel_de # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_db:
	pusha # push all registers
	call  _idt_intel_db # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_bp:
	pusha # push all registers
	call  _idt_intel_bp # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_of:
	pusha # push all registers
	call  _idt_intel_of # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_br:
	pusha # push all registers
	call  _idt_intel_br # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_ud:
	pusha # push all registers
	call  _idt_intel_ud # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_nm:
	pusha # push all registers
	call  _idt_intel_nm # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_df:
	pusha # push all registers
	call  _idt_intel_df # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_ts:
	pusha # push all registers
	call  _idt_intel_ts # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_np:
	pusha # push all registers
	call  _idt_intel_np # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_ss:
	pusha # push all registers
	call  _idt_intel_ss # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_gp:
	pusha # push all registers
	call  _idt_intel_gp # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_pf:
	pusha # push all registers
	call  _idt_intel_pf # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_mf:
	pusha # push all registers
	call  _idt_intel_mf # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_ac:
	pusha # push all registers
	call  _idt_intel_ac # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_mc:
	pusha # push all registers
	call  _idt_intel_mc # call C handler
	popa  # pop all registers
	iret  # return from interrupt

idt_intel_xf:
	pusha # push all registers
	call  _idt_intel_xf # call C handler
	popa  # pop all registers
	iret  # return from interrupt
