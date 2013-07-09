/*
 * TI DaVinci EVM board support
 *
 * Author: Kevin Hilman, MontaVista Software, Inc. <source@mvista.com>
 *
 * 2007 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>

#include <asm/setup.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/hardware.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/flash.h>

#include <asm/arch/common.h>

#include <asm/arch/irqs.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>

#include <asm/arch/debug_uart.h>

/* other misc. init functions */
void __init newchip_irq_init(void);
void __init newchip_map_common_io(void);

static struct plat_serial8250_port serial_platform_data[] = {
	{
		.membase = (char *) IO_ADDRESS(UART0_BASE),
		.mapbase = (unsigned long) UART0_BASE,
		.irq = IRQ0_UART0,
		.flags = UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
		.iotype = UPIO_MEM,
		.regshift = 2,
		.uartclk = 50000000,
	},
	{
		.membase = (char *) IO_ADDRESS(UART1_BASE),
		.mapbase = (unsigned long) UART1_BASE,
		.irq = IRQ1_UART1,
		.flags = UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
		.iotype = UPIO_MEM,
		.regshift = 2,
		.uartclk = 50000000,
	},
	{
		.flags = 0
	},
};

static struct platform_device serial_device = {
	.name = "serial8250",
	.id = 0,
	.dev = {
		.platform_data = serial_platform_data,
	},
};


static struct platform_device *newchip_evm_devices[] __initdata = {
	&serial_device,
};

static void __init
newchip_evm_map_io(void)
{
	newchip_map_common_io();
}

static __init void newchip_evm_init(void)
{
	uart_puts("newchip_evm_init\n");
	platform_add_devices(newchip_evm_devices,
			     ARRAY_SIZE(newchip_evm_devices));
}

static __init void newchip_evm_irq_init(void)
{
	uart_puts("newchip_evm_irq_init\n");
	newchip_irq_init();
}

MACHINE_START(NEWCHIP_EVM, "NewChip EVM")
	/* Maintainer: MontaVista Software <source@mvista.com> */
	.phys_io      = IO_PHYS,
	.io_pg_offst  = (io_p2v(IO_PHYS) >> 18) & 0xfffc,
	.boot_params  = (NEWCHIP_DDR_BASE + 0x100),
	.map_io	      = newchip_evm_map_io,
	.init_irq     = newchip_evm_irq_init,
	.timer	      = &newchip_timer,
	.init_machine = newchip_evm_init,
MACHINE_END
