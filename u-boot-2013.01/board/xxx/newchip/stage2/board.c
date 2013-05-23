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
#include "common.h"

#ifdef CFG_PRINTF
const char version_string[] =
	"UR0431A 0.1";
#endif


int check_header(xl_header * head)
{
	TRACE(KERN_DEBUG,"header:tag = 0x%x\n", *(unsigned int*)(head->tag));
	TRACE(KERN_DEBUG,"header:offset = 0x%x\n", head->offset);
	TRACE(KERN_DEBUG,"header:entry = 0x%x\n", head->entry);
	TRACE(KERN_DEBUG,"header:size = 0x%x\n", head->size);

	if(CFG_HEAD_TAG != *(unsigned int*)(head->tag))
	{
		TRACE(KERN_ERROR,"No TAG\n");
		return 1;
	}

	if(head->header_cksum != checksum32((void*)head, XL_HEAD_SIZE-sizeof(int)))
	{
                TRACE(KERN_ERROR,"Head cksum err\n");
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

