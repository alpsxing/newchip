/*
 * linux/arch/arm/mach-tcc8900/devices.c
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Feb, 2009
 * Description:
 *
 * Copyright (C) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>

#include <mach/debug_uart.h>
/*----------------------------------------------------------------------
 * Device     : SPI(GPSB) Master resource
 * Description: 
 *----------------------------------------------------------------------*/
#if defined(CONFIG_SPI_NEWCHIP)
static struct resource spi0_resources[] = {
    [0] = {
		.start = BSPI_BASE,
		.end   = BSPI_BASE + 48,
		.flags = IORESOURCE_MEM,
    }
};

static struct platform_device newchip_spi0_device = {
    .name			= "newchip_spi",
    .id				= 0,
    .resource		= spi0_resources,
    .num_resources	= ARRAY_SIZE(spi0_resources),
};

static inline void newchip_init_spi(void)
{
	platform_device_register(&newchip_spi0_device);
}
#endif

#if defined(CONFIG_I2C_DESIGNWARE)
/*----------------------------------------------------------------------
 * Device     : I2C resource
 * Description: tcc8900_i2c_resources has master0 and master1
 *----------------------------------------------------------------------*/
static struct resource newchip_i2c_resources[] = {
	[0] = {
		.start	= I2C_BASE,			/* I2C master ch0 base address */
		.end	= I2C_BASE+0xA8,			/* I2C master ch1 base address */
		.flags	= IORESOURCE_MEM,
    },
    [1]={
		.start = IRQ2_I2C,
		.end   = IRQ2_I2C,
		.flags = IORESOURCE_IRQ,
	}
};

static struct platform_device newchip_i2c_device = {
    .name           = "i2c_designware",
    .id             = 0,
    .resource       = newchip_i2c_resources,
    .num_resources  = ARRAY_SIZE(newchip_i2c_resources),
};

static inline void newchip_init_i2c(void)
{
    platform_device_register(&newchip_i2c_device);
}
#endif

#ifdef CONFIG_DW_WATCHDOG
static struct resource newchip_wdt_resources[] = {
	{
		.start	= IO_ADDRESS(WDT_BASE),
		.end	= IO_ADDRESS(WDT_BASE) + 256,
		.flags	= IORESOURCE_MEM,
	}
};

static struct platform_device newchip_wdt_device = {
	.name		= "dw_wdt",
	.id		= -1,
	.resource	= newchip_wdt_resources,
	.num_resources	= ARRAY_SIZE(newchip_wdt_resources),
};

static void __init newchip_add_device_watchdog(void)
{
	platform_device_register(&newchip_wdt_device);
}
#endif
/*
 * This gets called after board-specific INIT_MACHINE, and initializes most
 * on-chip peripherals accessible on this board (except for few like USB):
 *
 *  (a) Does any "standard config" pin muxing needed.  Board-specific
 *  code will have muxed GPIO pins and done "nonstandard" setup;
 *  that code could live in the boot loader.
 *  (b) Populating board-specific platform_data with the data drivers
 *  rely on to handle wiring variations.
 *  (c) Creating platform devices as meaningful on this board and
 *  with this kernel configuration.
 *
 * Claiming GPIOs, and setting their direction and initial values, is the
 * responsibility of the device drivers.  So is responding to probe().
 *
 * Board-specific knowlege like creating devices or pin setup is to be
 * kept out of drivers as much as possible.  In particular, pin setup
 * may be handled by the boot loader, and drivers should expect it will
 * normally have been done by the time they're probed.
 */
static int __init newchip_init_devices(void)
{
#if defined(CONFIG_SPI_NEWCHIP)
    uart_puts("newchip_init_spi\n");
	newchip_init_spi();
#endif
#if defined(CONFIG_I2C_DESIGNWARE)
    uart_puts("newchip_init_i2c\n");
	newchip_init_i2c();
#endif
#if defined(CONFIG_DW_WATCHDOG)
    uart_puts("newchip_add_device_watchdog\n");
	newchip_add_device_watchdog();
#endif
	return 0;
}

arch_initcall(newchip_init_devices);
