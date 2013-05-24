/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include "config.h"
#include "common.h"
#include "uart.h"
#include "arm_defines.h"

#define LCRVAL LCR_8N1					/* 8 data, 1 stop, no parity */
#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR)	/* Clear & enable FIFOs */


#define com_port ((UR0431A_UART_t)UART0_BASE)	

int uart_init(int baudrate)
{
	unsigned int baud_divisor;
	
	baud_divisor = CFG_APB_CLOCK / 16 / baudrate;	

	if(CFG_APB_CLOCK > (baud_divisor * 16 * baudrate))
		baud_divisor +=((CFG_APB_CLOCK/16 - baud_divisor * baudrate)*2 > baudrate) ? 1 : 0;


	com_port->ier = 0x00;
	com_port->lcr = LCR_BKSE | LCRVAL;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlh = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
	com_port->fcr = FCRVAL;
	
	return 0;
}

void uart_putc(char c)
{

	while ((com_port->lsr & LSR_THRE) == 0);
	if (c == '\n')
	{
		com_port->thr = '\r';	
		while ((com_port->lsr & LSR_THRE) == 0);
		com_port->thr = '\n';
	}
	else	
		com_port->thr = c;

}

void uart_putchar(char c)
{

        while ((com_port->lsr & LSR_THRE) == 0);
                com_port->thr = c;

}

int uart_getc (void)
{
	while ((com_port->lsr & LSR_DR) == 0);
	return (com_port->rbr);
}

void uart_puts (const char *s)
{
	while (*s) {
		uart_putc (*s++);
	}
}
#ifndef MINI_TEST
static void rx(void *addr, unsigned int size)
{
	void *end = addr + size;

	while(addr < end) {
		*(unsigned char *)addr = uart_getc();
		addr++;
	}
}

static unsigned short wait_cmd(void)
{
	unsigned char c;
	unsigned short cmd;

	cmd  = uart_getc();
	c    = uart_getc();
	cmd |= (c << 8);
	return cmd;
}

static void reply(unsigned short status)
{
	uart_putchar(status);
	uart_putchar(status >> 8);
}

unsigned int rx4(void)
{
	unsigned char c;
	unsigned int ret = 0;

	c = uart_getc();
	ret = c;
	c = uart_getc();
	ret |= (c << 8);
	c = uart_getc();
	ret |= (c << 16);
	c = uart_getc();
	ret |= (c << 24);
	return ret; 
}

static void do_set_baudrate(void)
{
	unsigned int	baudrate = rx4();
	uart_init(baudrate);
	u_delay(1000000);
	reply(E_OK);
}

static void do_call(void)
{
	void(*addr) (void);


	addr  = (void *) rx4();

	reply(E_OK);
	TRACE(KERN_INFO,"Go to 0x%x\n", addr);
	addr();
}

static void tx(void *addr, unsigned int size)
{
	unsigned char c;
	void *end = addr + size;

	while(addr < end) {
		c = *(unsigned char *) addr;
		addr++;
		uart_putchar(c);
	}
}

static void do_upload(void)
{
	void * addr;
	unsigned int size;
	unsigned int cksum;


	addr       = (void *) rx4();
	size       = rx4();
	cksum = checksum32(addr, size);

	reply(E_OK);
	TRACE(KERN_INFO,"Upload %d bytes at 0x%x\n", size, addr);
	tx(&cksum, 4);
	tx(addr, size);
}


static void do_download(void)
{
	void * addr;
	unsigned int size;
	unsigned int cksum;

	addr       = (void *) rx4();
	size       = rx4();
	cksum = rx4();

	rx(addr, size);
	if(cksum != checksum32(addr, size)) {
		reply(E_CHECKSUM);
	}
	reply(E_OK);
	TRACE(KERN_INFO,"Download %d bytes at 0x%x\n", size, addr);
}


int uart_boot(void)
{
	while(1) {
		switch(wait_cmd()) {
		case XCMD_CONNECT:
			reply(E_OK);
			break;
		case XCMD_SET_BAUD:
			do_set_baudrate();
			break;
		case XCMD_DOWNLOAD:
			do_download();
			break;
		case XCMD_UPLOAD:
			do_upload();
			break;
		case XCMD_SOC_CALL:
			do_call();
			break;
		default:
			reply(E_BADCMD);
		}
	}
	return 0;
}
#endif

