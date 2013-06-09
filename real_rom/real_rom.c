/*
 * Copyright (C) 2005 Texas Instruments.
 *
 * (C) Copyright 2004
 * Jian Zhang, Texas Instruments, jzhang@ti.com.
 *
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
#include "real_rom.h"

#ifdef COSIM
   #include "dsd_cpu.h"
#endif

#ifdef CFG_PRINTF
const char version_string[] =
	"UR0431A 0.1";
#endif

void start_armboot (void)
{
	unsigned int status;
	unsigned int rom_boot = 1;

	rom_boot = (readl(CFG_BOOT_MODE) &0x3c) >> 2 ;

	uart_init((unsigned int)CFG_UART_BAUD_RATE);

    TRACE(KERN_INFO, "\n\n%s\n", version_string);

	switch (rom_boot) {
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
#ifdef COSIM
	        TRACE(KERN_INFO,"SPI\n");
	        Display("MSG:SPI");
#else
	        TRACE(KERN_INFO,"SPI\n");
#endif
	        status = spi_boot();
			
	    //if spi_boot success, it should never return; 
	    //if it returns, we should try UART, so no break here
        case 0xf:
#ifdef COSIM
	        TRACE(KERN_INFO,"UART\n");   
            Display("MSG:UART");
#else
	        TRACE(KERN_INFO,"UART\n");   
#endif
	        uart_boot();
        default:
            break;
	}
	
	TRACE(KERN_ERROR,"No Boot Source\n");
	while(1);

}

int check_header(xl_header * head)
{
        unsigned int act_sum, exp_sum;

        TRACE(KERN_DEBUG,"header:tag = 0x%x\n", *(unsigned int*)(head->tag));
        TRACE(KERN_DEBUG,"header:offset = 0x%x\n", head->offset);
        TRACE(KERN_DEBUG,"header:entry = 0x%x\n", head->entry);
        TRACE(KERN_DEBUG,"header:size = 0x%x\n", head->size);
 
	if(((unsigned int) CFG_HEAD_TAG) != *(unsigned int*)(head->tag))
	{
		TRACE(KERN_ERROR,"No TAG act=0x%x  exp=0x%x\n", *(unsigned int*)(head->tag), (unsigned int) CFG_HEAD_TAG);
		return 1;
	}

	act_sum = (checksum32((void*)head, XL_HEAD_SIZE-sizeof(int)));
	exp_sum = head->header_cksum;

	if(exp_sum != act_sum) 
	{
                TRACE(KERN_ERROR,"Head cksum err exp:%x act:%x\n", exp_sum, act_sum);
                return 1;
	}

	return 0;
}

unsigned int checksum32(void *addr, unsigned int size)
{
	unsigned int sum;

	sum = 0;
	while(size >= 4) {
		sum  += * (unsigned int *) addr;
		addr += 4;
		size -= 4;
	}
	switch(size) {
	case 3:
		sum += (*(unsigned char *)(2+addr)) << 16;
	case 2:
		sum += (*(unsigned char *)(1+addr)) << 8;
	case 1:
		sum += (*(unsigned char *)(0+addr));
	}
	return sum;
}

void enter_entry(void* buf)
{
        TRACE(KERN_INFO, "Booting X-loader at 0x%x\n", buf);
        (*(void(*)())buf)();
        while(1);
}
