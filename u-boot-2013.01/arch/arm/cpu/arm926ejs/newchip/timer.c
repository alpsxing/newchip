/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * (C) Copyright 2009
 * Ilya Yanok, Emcraft Systems Ltd, <yanok@emcraft.com>
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

#include <common.h>
#include <div64.h>
#include <asm/io.h>

#include "timer.h"

int timer_init(void)
{

	return 0;
}

unsigned long long get_ticks(void)
{
	return 0;
}

ulong get_timer_masked(void)
{
	return 0;
}

ulong get_timer(ulong base)
{
	return 0;
}

/* delay x useconds AND preserve advance timstamp value */
void __udelay(unsigned long us)
{

	int count;
	volatile int *pcount = &count;
#if 0
        *TIMER_1_CTRL = 0;

        *TIMER_1_LOAD_VALUE = 0xFFFFFFFF;

        *TIMER_1_CTRL = 1;

	while(1)
	{
		if((0xFFFFFFFF - *TIMER_1_CUR_VALUE) > (us*TICK_PER_USECOND))
		{
			*TIMER_1_CTRL = 0;
			return;
		}
	}	
#endif	
	us = us*100;
	while(us > 0)
	{
		us--;
		*pcount++;
	}
}

ulong get_tbclk(void)
{
}
