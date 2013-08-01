/*
 * DaVinci interrupt controller definitions
 *
 *  Copyright (C) 2006 Texas Instruments.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef __ASM_ARCH_IRQS_H
#define __ASM_ARCH_IRQS_H

#define IRQ_REG_INTEN_L            0x00//IRQ interrupt source enable register (low)
#define IRQ_REG_INTEN_H            0x04//IRQ interrupt source enable register (high)
#define IRQ_REG_INTMASK_L          0x08//IRQ interrupt source mask register (low).
#define IRQ_REG_INTMASK_H          0x0c//IRQ interrupt source mask register (high).
#define IRQ_REG_INTFORCE_L         0x10//IRQ interrupt force register
#define IRQ_REG_INTFORCE_H         0x14//
#define IRQ_REG_RAWSTATUS_L        0x18//IRQ raw status register
#define IRQ_REG_RAWSTATUS_H        0x1c//
#define IRQ_REG_STATUS_L           0x20//IRQ status register
#define IRQ_REG_STATUS_H           0x24//
#define IRQ_REG_MASKSTATUS_L       0x28//IRQ interrupt mask status register
#define IRQ_REG_MASKSTATUS_H       0x2c//
#define IRQ_REG_FINALSTATUS_L      0x30//IRQ interrupt final status
#define IRQ_REG_FINALSTATUS_H      0x34 
#define FIQ_REG_INTEN              0xc0//Fast interrupt enable register
#define FIQ_REG_INTMASK            0xc4//Fast interrupt mask register
#define FIQ_REG_INTFORCE           0xc8//Fast interrupt force register
#define FIQ_REG_RAWSTATUS          0xcc//Fast interrupt source raw status register
#define FIQ_REG_STATUS             0xd0//Fast interrupt status register
#define FIQ_REG_FINALSTATUS        0xd4//Fast interrupt final status register
#define IRQ_REG_PLEVEL             0xd8//IRQ System Priority Level Register

#define NR_IRQS		32	

#define IRQ0_UART0	0
#define IRQ1_UART1	1
#define IRQ2_I2C	2                                                     
#define IRQ3_GPIO0	3
#define IRQ4_GPIO1      4                                               
#define IRQ5_GPIO2	5                                                     
#define IRQ6_GPIO3 	6                                                    
#define IRQ7_TIMER0 	7                                                    
#define IRQ8_TIMER0 	8                                                    
#define IRQ9_TIMER0 	9                                                    
#define IRQ10_IIMTER0 	10                                                    
#define IRQ11_SSI 	11                                                    
#define IRQ12_DMAC	12
#define IRQ13_WDT 	13                                                    


#define	FIQ0_HINOC_int0	0                                                     
#define FIQ1_HINOC_int1	1                                                   
#define FIQ2_HINOC_int2	2                                                    
#define FIQ3_HINOC_int3	3                                                     
#define FIQ4_DDR2	4
#define FIQ5_AHB	5
#define FIQ6_GMAC	6

#endif /* __ASM_ARCH_IRQS_H */
