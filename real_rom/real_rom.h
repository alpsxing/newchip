/*
 * (C) Copyright 2004
 * Texas Instruments
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __REAL_ROM_H_
#define __REAL_ROM_H_	


typedef unsigned char		uchar;
typedef volatile unsigned long	vu_long;
typedef volatile unsigned short vu_short;
typedef volatile unsigned char	vu_char;

#define asmlinkage	/* nothing */

#define NULL (0)

#define readl(a)       (*((volatile unsigned int *)(a)))
#define writel(v,a)    (*((volatile unsigned int *)(a)) = (v))


#ifdef	CFG_PRINTF
#define printf(fmt,args...)	uart_printf (fmt ,##args)
#define getc() uart_getc()
#define KERN_ERROR    1
#define KERN_INFO     2
#define KERN_DEBUG    3
#define TRACE(level,fmt,args...)  ( ((level) <= CURRENT_DEBUG_LEVEL ) ? printf(fmt,##args) : 12345678UL)
#else
#define printf(fmt,args...)
#define getc() ' '
#endif	/* CFG_PRINTF */
typedef struct {
	char	     tag[4];       //XFUN
	unsigned int offset;	   //payload start byte (from header start address)
	unsigned int entry; 	   //where to place the binary 
	unsigned int size;	   //size of binary 
	unsigned int loader_cksum; //chsum of binary
	unsigned int header_cksum; //cksum of first 16 bytes of header
}xl_header;

#define XL_HEAD_SIZE (sizeof(xl_header))

/* board */
int 	board_init (void);
int check_header(xl_header * head);
unsigned int checksum32(void *addr, unsigned int size);
void enter_entry(void* buf);
/* cpu */
int 	cpu_init (void);

/* lib/printf.c */
void    uart_printf (const char *fmt, ...);

/* driver/uart.c */
#define E_OK        ('O' + 'K' * 256)
#define E_CHECKSUM  ('E' + '1' * 256)
#define E_BADCMD    ('E' + '2' * 256)
#define E_TIMEOUT   ('E' + '3' * 256)
#define E_DEBUG     ('D' + 'B' * 256)

#define XCMD_CONNECT      ('H' + 'I' * 256)
#define XCMD_SET_BAUD     ('S' + 'B' * 256)
#define XCMD_DOWNLOAD     ('D' + 'L' * 256)
#define XCMD_UPLOAD       ('U' + 'L' * 256)
#define XCMD_SOC_MEMSET   ('M' + 'S' * 256)
#define XCMD_SOC_CALL     ('B' + 'L' * 256)

int uart_init(unsigned int baudrate);
int uart_boot(void);

/* driver/spi.c */
int spi_init(void);
int spi_boot(void);

/* driver/timer.c */
void u_delay(unsigned int us);

#endif	/* __REAL_ROM_H_ */
