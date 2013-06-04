.set RESET,          0x00000000

.global	UndefIsr
.global	SWIIsr
.global	ProgAbortIsr
.global	DataAbortIsr
.global	IRQIsr
.global	FIQIsr
.global _init
.arm


;###############################################################################
;# Standard definitions of mode bits and interrupt (I & F) flags in PSRs  
.org RESET
reset:
    LDR   pc, =_init                     ;# reset - _init   
    LDR   pc, =UndefIsr                  ;# undefined - _undf
    LDR   pc, =SWIIsr                    ;# SWI - _swi
    LDR   pc, =ProgAbortIsr              ;# i-prefetch abort - _pabt
    LDR   pc, =DataAbortIsr              ;# data abort
    NOP                                  ;# reserved
    LDR   pc, =IRQIsr                    ;# IRQ - read the VIC
    LDR   pc, =FIQIsr                    ;# FIQ - _fiq
    NOP
    NOP

