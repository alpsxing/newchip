#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_SKIP_LOWLEVEL_INIT	/* U-Boot is a 3rd stage loader */
/*
 * SoC Configuration
 */
#define CONFIG_ARM926EJS			/* arm926ejs CPU core */
#define CONFIG_SYS_HZ			1000


/* Memory Info */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			0x20000
#define PHYS_SDRAM_1_SIZE		(128 << 20)	/* 128 MiB */
#define CONFIG_MAX_RAM_BANK_SIZE	(256 << 20)	/* 256 MB */

#define CONFIG_SYS_MALLOC_LEN		(0x10000 + 512 * 1024)
/* memtest start address */
#define CONFIG_SYS_MEMTEST_START	0x7020000
#define CONFIG_SYS_MEMTEST_END		0x8020000	/* 16MB RAM test */

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR		\
	(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)

/* Serial Driver info: UART0 for console  */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	-4
#define CONFIG_SYS_NS16550_COM1		0x301200000
#define CONFIG_SYS_NS16550_CLK		18432000	
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200


/* U-Boot command configuration */
#define CONFIG_SYS_NO_FLASH		1
#define CONFIG_ENV_IS_NOWHERE	1

#define CONFIG_ENV_SIZE			32768
#define CONFIG_SYS_MAXARGS		32
#define CONFIG_SYS_LOAD_ADDR		0x820000		/* kernel address */
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size  */
#define CONFIG_SYS_PROMPT	"newchip>"	/* Monitor Command Prompt */
#define CONFIG_SYS_PBSIZE			/* Print buffer size */ \
		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR		\
	(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)

#endif

