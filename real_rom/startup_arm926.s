;#.text 0

;# Note that subsections (.text N) so that multiple literal pools will be
;# created, this helps prevent assembly time errors regarding literal pool too
;# far

.global _Stack
.global	UndefIsr
.global	SWIIsr
.global	ProgAbortIsr
.global	DataAbortIsr
.global	IRQIsr
.global	FIQIsr
.global _init
.align 4
.arm

;# These are used as origins for various subsections of the text section. This
;# defines the memory map for this testcase. The location of the debug subsection
;# must be coordinated with the SystemVerilog monitor
;#.set RESET,          0x00000000

;#.set DABT,           0x08000050
;#.set IRQ,            0x08000090
;#.set FIQ,            0x080000c0
;#.set SWI,            0x08000110 
;#.set UNDEF,          0x08000140
;#.set PABT,           0x08000170
;#.set STACK,          0x080003e0
;#.set MONITOR,        0x08000400
;#.set MESSAGES,       0x08000700

.set AHB_SRAM_BASE,  0x08000000
.set MONITOR_AHB_SRAM, 0x08000400
.set EXT_SRAM_BASE,  0x08003ff0

;# Data constants	
.set IDCODE,         0x41059461
.set CACHE_TYPE,     0x0f192192
.set TCM_SIZE,       0x00180180

;# Stack Sizes 
.set  UND_STACK_SIZE, 0x00000040 ;#
.set  ABT_STACK_SIZE, 0x00000040
.set  FIQ_STACK_SIZE, 0x00000040 ;#
.set  IRQ_STACK_SIZE, 0X00000040
.set  SVC_STACK_SIZE, 0x00000040
;# Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs 
.set  MODE_USR, 0x10            ;# User Mode 
.set  MODE_FIQ, 0x11            ;# FIQ Mode 
.set  MODE_IRQ, 0x12            ;# IRQ Mode 
.set  MODE_SVC, 0x13            ;# Supervisor Mode 
.set  MODE_ABT, 0x17            ;# Abort Mode 
.set  MODE_UND, 0x1B            ;# Undefined Mode 
.set  MODE_SYS, 0x1F            ;# System Mode 

.equ  NO_IRQ, 0x80               ;# when I bit is set, IRQ is disabled 
.equ  NO_FIQ, 0x40               ;# when F bit is set, FIQ is disabled 
.equ  NO_INT, (NO_IRQ | NO_FIQ)  ;# neither interrupt
.equ FORCE_FAIL, 0

;# This macro is intended to be used where it is required to trigger the
;# Arm1176Monitor from ASM code. An example scenario would be from sv_asm_int.s
.macro MONITOR_MSG MSGLOC
  STMDB sp!, {r0,r10}           ;# push regs used here locally
  LDR r10, =Monitor             ;# r10 loaded w/ pointer to monitor code
  LDR r0, =\MSGLOC              ;# message location into r0
  BLX r10                       ;# call, return
  LDMIA sp!, {r0,r10}           ;# restore context
.endm

;###############################################################################	
;# Primary ASM entry point.
;# Must setup a stack for each mode - note that this only sets up a usable stack 
;# for system/user, SWI and IRQ modes.   Also each mode is setup with 
;# FIQ interrupts initially disabled.  
;###############################################################################
_init:

    ;# initialize registers, careful not to step on r13-r15
    ;# r13=sp, r14=lr, r15=pc
    MOV r0,  #0 
    MOV r1,  #0 
    MOV r2,  #0 
    MOV r3,  #0 
    MOV r4,  #0 
    MOV r5,  #0 
    MOV r6,  #0 
    MOV r7,  #0 
    MOV r8,  #0 
    MOV r9,  #0 
    MOV r10, #0 
    MOV r11, #0
    MOV r12, #0
    	
    ;# R13 is the stack pointer used by PUSH and POP
    ;# set it 
    LDR r0, =__SP_UNDEF

    ;# Setup the CPSR, see TRM A2.5
    ;# note we aren't setting the I_BIT, allowing IRQ's to be enabled.
    MSR   CPSR_c, #MODE_UND|NO_FIQ ;# Undefined Instruction Mode 
    MOV   sp, r0
    SUB   r0, r0, #UND_STACK_SIZE
    MSR   CPSR_c, #MODE_ABT|NO_FIQ ;# Abort Mode 
    MOV   sp, r0
    SUB   r0, r0, #ABT_STACK_SIZE
    MSR   CPSR_c, #MODE_FIQ|NO_FIQ ;# FIQ Mode 
    MOV   sp, r0
    SUB   r0, r0, #FIQ_STACK_SIZE
    MSR   CPSR_c, #MODE_IRQ|NO_FIQ ;# IRQ Mode 
    MOV   sp, r0
    SUB   r0, r0, #IRQ_STACK_SIZE
    MSR   CPSR_c, #MODE_SVC|NO_FIQ ;# Supervisor Mode 
    MOV   sp, r0
    SUB   r0, r0, #SVC_STACK_SIZE
    MSR   CPSR_c, #MODE_SYS|NO_FIQ ;# System Mode 
    MOV   sp, r0


    ;# Write CP15, R1,0,0 -- disable TBIT (thumb mode disable), disable TCM and caches, disable MMU
    LDR r10, =0x00008000
    MCR p15, 0, r10, c1, c0, 0

   ;#######################################################
    ;# initialize regs w/ random data; helpful to debug sims
    ;# no other reason...
    LDR r0,   =0x1f444400
    LDR r1,   =0xf2c0df6d
    LDR r2,   =0xfeebc22b
    LDR r3,   =0x564a45ee
    LDR r4,   =0x8859a2b2
    LDR r5,   =0xd738b56e
    LDR r6,   =0xa5ef8f6c
    LDR r7,   =0xcbfc9a26
    LDR r8,   =0x49e6d004
    LDR r9,   =0x7508ea4c
    LDR r10,  =0x2b46182d
    LDR r11,  =0x8f2cc72e
    LDR r12,  =0x8f2cc72e

    LDR r0, =CHKPT1a         ;# diag for debug
    BL Monitor               ;# call/return here

    ;# Branch to C code
    LDR   pc, =start_armboot
    NOP 	 
    NOP 
    NOP 
    NOP 
    NOP 
    NOP 
    NOP 
    NOP
    NOP 
    NOP 
    NOP
    NOP

;###############################################################################
;# This routine is intended to service a data abort. Expect the abort link
;# register to hold the return address. Generate a message and increment a
;# counter. Note that we are _not_ trying to re-do the aborting command,
;# probably only useful for simulation testcases.
;###############################################################################
;#.org DABT
DataAbortIsr:

  STMDB sp!, {r0 - r7}      ;# stack registers used in ISR
  MOV r6, lr                ;# pointer for ISR return (aka r14_irq)

  ;# increment the Abort counter, kept in Ext SRAM memory
  LDR  r7, =EXT_SRAM_BASE      ;# counter in Ext SRAM
  LDR  r8, [r7]             ;# read the counter
  ADD  r8, r8, #1           ;# incr the counter
  STR  r8, [r7]             ;# save back

  ;# Generate a message, will display the abort counter value in r8
  MONITOR_MSG AbortMessage

  ;# restore the return pointer from the entry here
  MOV lr, r6

  ;# pop registers, has to be the same as the push
  LDMIA sp!, {r0 - r7}

  ;# refer to Armv5 TRM section 2.6.6,. This is
  ;# returning from the abort ISR. Important - note only backing up one instr,
  ;# not redoing the aborted instr
  ;# SUBS pc, lr, #8
  SUBS pc, lr, #4

;###############################################################################
;# This routine is intended to service all regular IRQs. 
;###############################################################################
;#.org IRQ
IRQIsr:
  STMDB sp!, {r0 - r12}     ;# stack registers used in ISR
  MOV r7, lr                ;# pointer for ISR return (aka r14_irq)

  ;# Generate a message
  MONITOR_MSG IRQMessage

  ;# Call the test specific interrupt handler  
  ;# Always the same name,,, InterruptServiceRoutine.
  ;#BL InterruptServiceRoutine		
	
  ;# restore the return pointer from the entry here
  MOV lr, r7

  ;# pop registers, has to be the same as the push
  LDMIA sp!, {r0 - r12}

  ;# refer to Armv5 TRM section 2.6.8,. 
  SUBS pc, lr, #4
			
;###############################################################################
;# This routine is intended to service all fast FIQs.
;###############################################################################
;#.org FIQ
FIQIsr:
  STMDB sp!, {r0 - r7}      ;# stack registers used in ISR
  MOV r7, lr                ;# pointer for ISR return (aka r14_irq)
  MOV r8, #0                ;# Initialize FIQ mode registers
  MOV r9, #0
  MOV r10, #0
  MOV r11, #0
  MOV r12, #0

  ;# Generate a message
  MONITOR_MSG FIQMessage

  ;# Call the test specific interrupt handler  
  ;# Always the same name,,, InterruptServiceRoutine.
  ;#BL InterruptServiceRoutine		
	
  ;# restore the return pointer from the entry here
  MOV lr, r7

  ;# pop registers, has to be the same as the push
  LDMIA sp!, {r0 - r7} 

  ;# refer to Armv5 TRM section 2.6.8,. 
  SUBS pc, lr, #4
			
;###############################################################################
;# This routine is intended to service all software interrupts.
;###############################################################################
;#.org SWI
SWIIsr:
  STMDB sp!, {r0 - r12}     ;# stack registers used in ISR
  MOV r6, lr                ;# pointer for ISR return (aka r14_irq)

  ;# Generate a message
  MONITOR_MSG SWIMessage

  ;# Call the test specific interrupt handler  
  ;# Always the same name,,, SWIServiceRoutine.
  ;#BL SWIServiceRoutine
	
  ;# restore the return pointer from the entry here
  MOV lr, r6

  ;# pop registers, has to be the same as the push
  LDMIA sp!, {r0 - r12}

  ;# refer to Armv5 TRM section 2.6.8,. 
  MOV pc, r14
				
;###############################################################################
;# UNDEFINED INSTRUCTION TRAP
;###############################################################################
;#.org UNDEF
UndefIsr:

  STMDB sp!, {r0 - r12}     ;# stack registers used in ISR
  MOV r6, lr                ;# pointer for ISR return (aka r14_irq)

  ;# Generate a message
  MONITOR_MSG UndefMessage

  ;# Call the test specific interrupt handler  
  ;# Always the same name,,, UndefServiceRoutine.
  ;#BL UndefServiceRoutine
  ;#BL InterruptServiceRoutine		
	
  ;# restore the return pointer from the entry here
  MOV lr, r6

  ;# pop registers, has to be the same as the push
  LDMIA sp!, {r0 - r12}

  ;# refer to Armv5 TRM section 2.6.8,. 
  MOVS pc, r14

;###############################################################################
;# PROGRAM/INSTRUCTION FETCH ABORT TRAP
;###############################################################################
;#.org PABT
ProgAbortIsr:

  STMDB sp!, {r0 - r12}     ;# stack registers used in ISR
  MOV r6, lr                ;# pointer for ISR return (aka r14_irq)

  ;# Generate a message
  MONITOR_MSG PAbortMessage

  ;# Call the test specific interrupt handler  
  ;# Always the same name,,, UndefServiceRoutine.
  ;#BL IAbortServiceRoutine
  ;#BL InterruptServiceRoutine		
	
  ;# restore the return pointer from the entry here
  MOV lr, r6

  ;# pop registers, has to be the same as the push
  LDMIA sp!, {r0 - r12}

  ;# refer to Armv5 TRM section 2.6.8,. 
  MOVS pc, r14


;###############################################################################
;# reserve some space for the stack, shouldn't need it anyway
;#.text 1
;#.org STACK
_Stack:
;# put this string here to make it easy to see in the readmem output where the
;# stack is (initial debug)
.string "this is the stack"

.align
;###############################################################################
;# diagnostic, this coupled with the SystemVerilog Arm1176Monitor - allows
;# pass, fail, display. For a fail message, this function is expected to be
;# called with:
;#      LDR r0, =FAIL   ;# or other 4char string pointer
;#      bl Monitor
;# 
;# arg1:   r0    - location for fail, pass, or mesg message
;# arg2-3: r1-r3 - arbitrary data, monitor can be extended to look at these
;# 
;#.text 2
;#.org MONITOR
Monitor:
    ;# push registers
    NOP	
       .global MonitorC
MonitorC:
    STMDB sp!, {r1 - r3, lr}    ;# push regs on the stack

    ;# give r1 a distinctive value so we can know a mesg was loaded
    LDR r1, =0xeeeeeeee

    ;# the SystemVerilog monitor expects the r1 register to contain the
    ;# message pointer, so copy arg0 there. Load r1 last so the monitor knows
    ;# the message is ready
    MOV r1, r0

    ;# pop registers, has to be the same as the push
    LDMIA sp!, {r1 - r3, lr}

    ;# return by branching to the link register
    BX lr
	
       .global _FAIL
_FAIL:
    LDR r0, =FAIL       ;# to get here, we must have failed
    BL Monitor          ;# call/return here

       .global _PASS
_PASS:
    LDR r0, =PASS       ;# to get here, we must have failed
    BL Monitor          ;# call/return here

       .global _END
_END:
    B .                 ;# loop on same instruction

		
;#######################################################
;# Useful routines, directly callable from C
;#######################################################
;#.text 3
;#.org ROUTINES

	
;#;###############################################################################
;#;# This inclusion implements a software interface allowing SystemVerilog to
;#;# call ASM routines.
;#;# This also includes basic vector interrupt handling support for C based code
;###############################################################################
;#.text 3
;#.include "sv_asm.s"
;#
;#.text 4
;#.include "sv_asm_int.s"
;#
;#;# manually place the literal org pool here to prevent an assembly time error
;#;# (literal pool to far away).
;#.ltorg

;###########################################################
;#.text 3
;#.org MESSAGES	    

;###########################################################
;# monitor will discern a test fail (end) 
FAIL: .asciz "FAIL"

;# monitor will discern a test pass (end)
PASS: .asciz "PASS"

;# monitor will check/display other regs
DUMP: .asciz "DUMP"

;# monitor will toggle the remap signal
REMAP: .asciz "RMAP"

;# display the abort counter
AbortMessage: .asciz "Abort: count = %r8"

;# display the irq message
IRQMessage: .asciz "ASM: Entering IRQ handler"

;# display the irq message
FIQMessage: .asciz "ASM: Entering FIQ handler"

;# display the SWI message
SWIMessage: .asciz "ASM: Entering SWI handler"

;# display the SWI message
UndefMessage: .asciz "ASM: Entering UNDEF handler"

;# display the SWI message
PAbortMessage: .asciz "ASM: Entering Program Abort handler"

AsmMessage: .asciz "This is a monitor message from asm"

;# define messages to pass to the SystemVerilog Monitor. These have to be 4
;# character strings, and must be full-word aligned
CHKPT1a: .asciz "Chkpt 1"

