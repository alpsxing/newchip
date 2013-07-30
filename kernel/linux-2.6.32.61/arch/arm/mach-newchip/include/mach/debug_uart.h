
#ifndef UART_DEBUG_H
#define UART_DEBUG_H

#include <mach/hardware.h>
#include <mach/io.h>

struct UR0431A_UART {
	unsigned char rbr;		/* 0 */
	int pad1:24;
	unsigned char ier;		/* 1 */
	int pad2:24;
	unsigned char fcr;		/* 2 */
	int pad3:24;
	unsigned char lcr;		/* 3 */
	int pad4:24;
	unsigned char mcr;		/* 4 */
	int pad5:24;
	unsigned char lsr;		/* 5 */
	int pad6:24;
	unsigned char msr;		/* 6 */
	int pad7:24;
	unsigned char scr;		/* 7 */
	int pad8:24;
} __attribute__ ((packed));

#define thr rbr
#define iir fcr
#define dll rbr
#define dlh ier

typedef volatile struct UR0431A_UART *UR0431A_UART_t;

#define FCR_FIFO_EN     0x01		/* Fifo enable */
#define FCR_RXSR        0x02		/* Receiver soft reset */
#define FCR_TXSR        0x04		/* Transmitter soft reset */
#define MCR_DTR         0x01
#define MCR_RTS         0x02
#define MCR_DMA_EN      0x04
#define MCR_TX_DFR      0x08
#define LCR_WLS_MSK	0x03		/* character length slect mask */
#define LCR_WLS_5	0x00		/* 5 bit character length */
#define LCR_WLS_6	0x01		/* 6 bit character length */
#define LCR_WLS_7	0x02		/* 7 bit character length */
#define LCR_WLS_8	0x03		/* 8 bit character length */
#define LCR_STB		0x04		/* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define LCR_PEN		0x08		/* Parity eneble */
#define LCR_EPS		0x10		/* Even Parity Select */
#define LCR_STKP	0x20		/* Stick Parity */
#define LCR_SBRK	0x40		/* Set Break */
#define LCR_BKSE	0x80		/* Bank select enable */
#define LSR_DR		0x01		/* Data ready */
#define LSR_OE		0x02		/* Overrun */
#define LSR_PE		0x04		/* Parity error */
#define LSR_FE		0x08		/* Framing error */
#define LSR_BI		0x10		/* Break */
#define LSR_THRE	0x20		/* Xmit holding register empty */
#define LSR_TEMT	0x40		/* Xmitter empty */
#define LSR_ERR		0x80		/* Error */
#define LCR_8N1		0x03



#define com_port ((UR0431A_UART_t)IO_ADDRESS(UART0_BASE))	

static void uart_putc(char c)
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

static void uart_puts (const char *s)
{
	while (*s) {
		uart_putc (*s++);
	}
}



#endif
