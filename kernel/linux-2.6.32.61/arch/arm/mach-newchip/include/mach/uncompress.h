/*
 * Serial port stubs for kernel decompress status messages
 *
 *  Author:     Anant Gole
 * (C) Copyright (C) 2006, Texas Instruments, Inc
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/types.h>
#include <linux/serial_reg.h>
#include <mach/hardware.h>

/* PORT_16C550A, in polled non-fifo mode */

static void putc(char c)
{
	volatile u32 *uart = (volatile void *) UART0_BASE;

	while (!(uart[UART_LSR] & UART_LSR_THRE))
		barrier();
	uart[UART_TX] = c;
}

static inline void flush(void)
{
	volatile u32 *uart = (volatile void *) UART0_BASE;
	while (!(uart[UART_LSR] & UART_LSR_THRE))
		barrier();
}

static void debug_putstr(const char *ptr)
{
	char c;

	while ((c = *ptr++) != '\0') {
		if (c == '\n')
			putc('\r');
		putc(c);
	}

	flush();
}


#define arch_decomp_setup()
#define arch_decomp_wdog()
