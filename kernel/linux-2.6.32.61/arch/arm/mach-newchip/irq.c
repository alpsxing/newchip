/*
 * Interrupt handler for DaVinci boards.
 *
 * Copyright (C) 2006 Texas Instruments.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/mach/irq.h>


#define which_irq_l(irq)         (0x01u << (irq)) 
#define which_irq_h(irq)         (0x01u << ((irq) & 0x1f)) 

#define write_irq_reg(addr, val)        __raw_writel(val, addr+(IO_ADDRESS(INTC_BASE))) 
#define read_irq_reg(addr)              __raw_readl(addr+(IO_ADDRESS(INTC_BASE))) 
#define set_irq_reg(addr, val)          write_irq_reg(addr, ((val) | read_irq_reg(addr)))
#define clear_irq_reg(addr, val)        write_irq_reg(addr, (~(val) & read_irq_reg(addr)))


static void newchip_irq_ack(u32 irq)
{
	//no irq ack
}

static void newchip_irq_mask(u32 irq)
{
    if (irq >= 32)
    {
        set_irq_reg(IRQ_REG_INTMASK_H, which_irq_h(irq));
    }
    else
    {
        set_irq_reg(IRQ_REG_INTMASK_L, which_irq_l(irq));
    }

}

static void newchip_irq_unmask(u32 irq)
{
    if (irq >= 32)
    {
       clear_irq_reg(IRQ_REG_INTMASK_H, which_irq_h(irq));
    }
    else
    {
       clear_irq_reg(IRQ_REG_INTMASK_L, which_irq_l(irq));
    }
}

static struct irq_chip newchip_irq_chip = {
	.name		= "newchip_irq",
	.ack		= newchip_irq_ack,
	.mask		= newchip_irq_mask,
	.unmask		= newchip_irq_unmask,
};


/*
 * Initialize the AIC interrupt controller.
 */
void __init newchip_irq_init(void)
{
	u32 i;
	
	write_irq_reg(IRQ_REG_INTEN_L, 0xffffffff);//enable irq interrupt
	write_irq_reg(IRQ_REG_INTEN_H, 0xffffffff);
	write_irq_reg(IRQ_REG_INTMASK_L, 0xffffffff); //mask all irq interrupt
	write_irq_reg(IRQ_REG_INTMASK_H, 0xffffffff);
	write_irq_reg(IRQ_REG_INTFORCE_L, 0);
	write_irq_reg(IRQ_REG_INTFORCE_H, 0);
	write_irq_reg(FIQ_REG_INTEN, 0x03); //enable fiq interrupt
	write_irq_reg(FIQ_REG_INTMASK, 0x03); //mask fiq interrupt
	write_irq_reg(IRQ_REG_PLEVEL, 0);

	for (i = 0; i < NR_NEWCHIP_IRQS; i++) {
		set_irq_chip(i, &newchip_irq_chip);
		set_irq_handler(i, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);//no probe and auto enable		
	}
}

