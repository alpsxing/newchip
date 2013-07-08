#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_USE_SPIFLASH

#define CONFIG_SKIP_LOWLEVEL_INIT	/* U-Boot is a 3rd stage loader */
/*
 * SoC Configuration
 */
#define CONFIG_ARM926EJS			/* arm926ejs CPU core */
#define CONFIG_SYS_HZ			1000


/* Memory Info */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			0x10000000
#define PHYS_SDRAM_1_SIZE		(16 << 20)	/* 16 MiB */
#define CONFIG_MAX_RAM_BANK_SIZE	(256 << 20)	/* 256 MB */

#define CONFIG_SYS_MALLOC_LEN		(0x10000 + 512 * 1024)
/* memtest start address */
#define CONFIG_SYS_MEMTEST_START	0x10200000
#define CONFIG_SYS_MEMTEST_END		0x10400000	/* 2MB RAM test */

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR		\
	(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)

/* Serial Driver info: UART0 for console  */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	-4
#define CONFIG_SYS_NS16550_COM1		0x90020000
#define CONFIG_SYS_NS16550_CLK		50000000	
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200

#ifdef CONFIG_USE_SPIFLASH

#undef CONFIG_ENV_IS_IN_FLASH
#undef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SIZE			(256 << 10)
#define CONFIG_ENV_OFFSET		(15 << 20)
#define CONFIG_ENV_SECT_SIZE		(256 << 10)	
#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_FLASH
#define CONFIG_CMD_SPI
#define CONFIG_CMD_SF
#define CONFIG_CMD_SAVEENV

#define CONFIG_SPI
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_NEWCHIP_SPI
#define CONFIG_SYS_SPI_BASE		0x04000000

#else
/* U-Boot command configuration */
#define CONFIG_SYS_NO_FLASH		1
#define CONFIG_ENV_IS_NOWHERE	1
#define CONFIG_ENV_SIZE			32768
#endif

#define CONFIG_CMDLINE_TAG
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_LOADB

#define CONFIG_BOOTARGS "mem=32M console=ttyS0,115200n8"

#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_MAXARGS		32
#define CONFIG_SYS_LOAD_ADDR		0x10700000		/* kernel address */
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size  */
#define CONFIG_SYS_PROMPT	"newchip>"	/* Monitor Command Prompt */
#define CONFIG_SYS_PBSIZE			/* Print buffer size */ \
		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR		\
	(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)

#endif

