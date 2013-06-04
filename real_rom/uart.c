#include "config.h"
#include "real_rom.h"
#include "uart.h"

#define LCRVAL LCR_8N1					/* 8 data, 1 stop, no parity */
#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR)	/* Clear & enable FIFOs */


#define com_port ((UR0431A_UART_t)UART_0_BASE)	

int uart_init(unsigned int baudrate)
{
	unsigned int baud_divisor;
	
	baud_divisor = ((unsigned int)CFG_APB_CLOCK / 16) / baudrate;	

	if(CFG_APB_CLOCK > (baud_divisor * 16 * baudrate))
		baud_divisor +=((CFG_APB_CLOCK/16 - baud_divisor * baudrate)*2 > baudrate) ? 1 : 0;


	com_port->ier = 0x00;
	com_port->lcr = LCR_BKSE | LCRVAL; /*bank select switch*/
	com_port->dll = baud_divisor & 0xff;
	com_port->dlh = (baud_divisor >> 8) & 0xff;
#ifdef COSIM    
	com_port->dll = 1;
	com_port->dlh = 0;
#endif
	com_port->lcr = LCRVAL;
	com_port->fcr = FCRVAL;  /*soft reset*/

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

static void rx(void *addr, unsigned int size)
{
	void *end = addr + size;

	while(addr < end) {
		*(unsigned char *)addr = uart_getc();
		addr++;
	}
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

static unsigned short wait_cmd(void)
{
        unsigned char c;

	unsigned short cmd;

	cmd  = uart_getc();
	c    = uart_getc();
	cmd |= (c << 8);
  
	//	TRACE(KERN_INFO,"RCV %x = %x\n", cmd);

	return cmd;
}

static void reply(unsigned short status)
{
	uart_putchar(status);
	uart_putchar(status >> 8);
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
	cksum      = rx4();

	TRACE(KERN_INFO,"DL CMD\n");

	rx(addr, size);
	if(cksum != checksum32(addr, size)) {
		reply(E_CHECKSUM);
	}
	reply(E_OK);
#ifdef COSIM
	Display("MSG:DL Done");
#endif
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

/*end of file */

