/*
 * Clock and PLL control for DaVinci devices
 *
 * Copyright (C) 2006-2007 Texas Instruments.
 * Copyright (C) 2008-2009 Deep Root Systems, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include "clock.h"

static struct clk i2cclock = {
	.name		= "i2c_designware",
	.rate	= 400000
};

static struct clk wdtclock = {
	.name		= "dw_wdt",
	.rate	= 50000000
};

static struct clk spiclock = {
	.name		= "dw_spi",
	.rate	= 5000000
};

/* clocks cannot be de-registered no refcounting necessary */
struct clk *clk_get(struct device *dev, const char *id)
{
    printk("clk_get: %s\r\n", dev_name(dev));
    if(!strcmp(dev_name(dev), "i2c_designware.0")) {
        printk("Return i2cclock.\r\n");
        return &i2cclock;
    }
    else if(!strcmp(dev_name(dev), "dw_wdt")) {
        printk("Return wdtclock\r\n");
        return &wdtclock;
    }
    else if(!strcmp(dev_name(dev), "dw_spi_mmio.1")) {
        printk("Return spiclock\r\n");
        return &spiclock;
    }

    return NULL;
}
EXPORT_SYMBOL(clk_get);

void clk_put(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_put);

int clk_enable(struct clk *clk)
{
	return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
    return;
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
	if (clk == NULL || IS_ERR(clk))
		return -EINVAL;

	return clk->rate;
}
EXPORT_SYMBOL(clk_get_rate);

long clk_round_rate(struct clk *clk, unsigned long rate)
{
	if (clk == NULL || IS_ERR(clk))
		return -EINVAL;

	return clk->rate;
}
EXPORT_SYMBOL(clk_round_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	if (clk == NULL || IS_ERR(clk))
		return -EINVAL;

	/* changing the clk rate is not supported */
	return -EINVAL;
}
EXPORT_SYMBOL(clk_set_rate);

