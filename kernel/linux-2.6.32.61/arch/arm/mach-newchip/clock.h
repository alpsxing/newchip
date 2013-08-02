/*
 * TI DaVinci clock definitions
 *
 * Copyright (C) 2006-2007 Texas Instruments.
 * Copyright (C) 2008-2009 Deep Root Systems, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ARCH_ARM_NEWCHIP_CLOCK_H
#define __ARCH_ARM_NEWCHIP_CLOCK_H

struct clk {
	const char		*name;
	unsigned long		rate;
};


#endif
