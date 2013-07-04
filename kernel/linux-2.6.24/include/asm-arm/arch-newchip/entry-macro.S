/*
 * Low-level IRQ helper macros for TI DaVinci-based platforms
 *
 * Author: Kevin Hilman, MontaVista Software, Inc. <source@mvista.com>
 *
 * 2007 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#include <asm/arch/io.h>
#include <asm/arch/irqs.h>

   .macro  disable_fiq
    .endm

    .macro  get_irqnr_preamble, base, tmp
	    ldr		\base,=IO_ADDRESS(0x900a0000)	
    .endm

    .macro  arch_ret_to_user, tmp1, tmp2
    .endm

    .macro  get_irqnr_and_base, irqnr, irqstat, base, tmp
1001:    
        ldr     \irqstat, [\base, #IRQ_REG_FINALSTATUS_L]
        cmp     \irqstat, #0
        beq     1002f
        
        clz     \irqnr, \irqstat
        rsb     \irqnr, \irqnr, #31
        b       1003f

1002:
        ldr     \irqstat, [\base, #IRQ_REG_FINALSTATUS_H]   
        and     \irqstat, \irqstat, #0xff
        cmp     \irqstat, #0
        beq     1003f
        
        clz     \irqnr, \irqstat
        rsb     \irqnr, \irqnr, #31
        add     \irqnr, \irqnr, #32
1003:
    .endm
