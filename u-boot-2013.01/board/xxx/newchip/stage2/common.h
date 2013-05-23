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

#ifndef __COMMON_H_
#define __COMMON_H_	1
//#include "config.h"


typedef unsigned char		uchar;
typedef volatile unsigned long	vu_long;
typedef volatile unsigned short vu_short;
typedef volatile unsigned char	vu_char;


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
unsigned int strnlen(const char *s, unsigned int count);
#else
#define printf(fmt,args...)
#define getc() ' '
#endif	/* CFG_PRINTF */

//#define CFG_HEAD_TAG 'XFUN'
#define CFG_HEAD_TAG 0x4e554658

typedef struct {
	char	     tag[4];    // 'XFUN'
	unsigned int offset;	//
	unsigned int entry; 	//where to place 
	unsigned int size;	 //size of binary 
	unsigned int loader_cksum; //chsum of binary
	unsigned int header_cksum; //cksum of first 16 bytes of header
}xl_header;

#define XL_HEAD_SIZE (sizeof(xl_header))

/* board */
int 	board_init (void);
int check_header(xl_header * head);
unsigned int checksum32(void *addr, unsigned int size);

/* lib/printf.c */
void    uart_printf (const char *fmt, ...);

void u_delay(unsigned int us);
int uart_init(int baudrate);
void uart_puts (const char *s);
int uart_boot(void);
int bspi_test(void);
void DDR2_init_Test(void);
int mem_test(int ddr_base, int ddr_size );

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

// serial defines 
#define CFG_PRINTF
#define CFG_UART_BAUD_RATE 115200
#define CFG_PBSIZE 256

//spi defines
#define CFG_SPI_CLK 500000


#endif	/* __COMMON_H_ */
