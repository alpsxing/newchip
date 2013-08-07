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

#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <asm/setup.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <mach/common.h>

#include <mach/irqs.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>

#include <mach/debug_uart.h>

#include <linux/phy.h>
#include <linux/stmmac_plat.h>

#include <linux/spi/spi-dw.h>

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

static struct plat_stmmacphy_data phy_private_data = {
        .bus_id = 0,
        .phy_addr = 0,
        .phy_mask = 0,
        .interface = PHY_INTERFACE_MODE_MII,
};
 
static struct platform_device phy_device = {
        .name           = PHY_RESOURCE_NAME,
        .id             = 0,
        .dev = {
                .platform_data = &phy_private_data,
         }
};

static struct plat_stmmacenet_data eth_private_data = {
	.bus_id   = 0,
	.enh_desc = 1,
	.has_gmac = 1,
	.pbl = 32,
};

static u64 eth_dmamask = DMA_BIT_MASK(32);
static struct platform_device eth_device = {
	.name           = STMMAC_RESOURCE_NAME,
	.id             = 0,
	.num_resources  = 2,
	.resource       = (struct resource[]) {
		{
			.start  = MAC_BASE,
			.end    = MAC_BASE + 0x2000,
			.flags  = IORESOURCE_MEM,
		},
		{
			.name   = "macirq",
			.start  = 20,
			.end    = 20,
			.flags  = IORESOURCE_IRQ,
		},
	},
	.dev = {
		.dma_mask		= &eth_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data = &eth_private_data,
	}
};

static struct platform_device *newchip_evm_devices[] __initdata = {
	&serial_device,
	&phy_device,
	&eth_device,
};


/*
 *	ST M25P28 SPI NOR FLASH
*/

static struct mtd_partition newchip_M25P28_partitions[] = {
	{
		.name		= "stage2",
		.offset		= 0,
		.size		= SZ_256K,
		.mask_flags	= 0,
	},
	{
		.name		= "stage3",
		.offset		= SZ_256K,
		.size		= SZ_2M - SZ_256K,
		.mask_flags	= 0,
	},
	{
		.name		= "kernel",
		.offset		= SZ_2M,
		.size		= SZ_8M,
		.mask_flags	= 0,
	},
	{
		.name		= "data",
		.offset		= SZ_2M + SZ_8M,
		.size		= SZ_1M,
		.mask_flags	= 0,
	}
};

static struct flash_platform_data newchip_M25P28_info = {
	.name = "m25p128",
	.parts = newchip_M25P28_partitions,
	.nr_parts = ARRAY_SIZE(newchip_M25P28_partitions),
	.type = "m25p128"
};

static struct mtd_partition newchip_legacy_M25P28_partitions[] = {
	{
		.name		= "data",
		.offset		= 0,
		.size		= SZ_1M,
		.mask_flags	= 0,
	}
};

static struct flash_platform_data newchip_legacy_M25P28_info = {
	.name = "m25p128_legacy",
	.parts = newchip_legacy_M25P28_partitions,
	.nr_parts = ARRAY_SIZE(newchip_legacy_M25P28_partitions),
	.type = "m25p128"
};


static struct dw_spi_chip dw_spi_chip_info = {
	.poll_mode	= 1,
	.type	    = 0,
	.enable_dma	= 0,
    .cs_control = NULL
};


static struct spi_board_info newchip_spi_board_info[] = {
	[0] = {
		.modalias = "m25p128",
		.bus_num = 0,					// spi channel 0
		.chip_select = 0,

		.platform_data = &newchip_M25P28_info,
		.max_speed_hz = 20*1000*1000,	// default 2Mhz
		.mode = 0						// default 0, you can choose [SPI_CPOL|SPI_CPHA|SPI_CS_HIGH|SPI_LSB_FIRST]
	},
	[1] = {
		.modalias = "m25p80_legacy",
		.bus_num = 1,					// spi channel 0
		.chip_select = 0,
        .controller_data = &dw_spi_chip_info,
		.platform_data = &newchip_legacy_M25P28_info,
		.max_speed_hz = 5*1000*1000,	// default 2Mhz
		.mode = 0						// default 0, you can choose [SPI_CPOL|SPI_CPHA|SPI_CS_HIGH|SPI_LSB_FIRST]
	},
};

static void __init newchip_evm_map_io(void)
{
	newchip_map_common_io();
}

static __init void newchip_evm_init(void)
{
	uart_puts("newchip_evm_init\n");
	platform_add_devices(newchip_evm_devices, ARRAY_SIZE(newchip_evm_devices));
    spi_register_board_info(newchip_spi_board_info, ARRAY_SIZE(newchip_spi_board_info));
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
