/*
 * MTD SPI driver for ST M25Pxx (and similar) serial flash chips
 *
 * Author: Mike Lavender, mike@steroidmicros.com
 *
 * Copyright (c) 2005, Intec Automation Inc.
 *
 * Some parts are based on lart.c by Abraham Van Der Merwe
 *
 * Cleaned up and generalized based on mtd_dataflash.c
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <linux/spi/spi.h>
#include <linux/spi/flash.h>



#define FLASH_PAGESIZE		256

/* Flash opcodes. */
#define	OPCODE_WREN		0x06	/* Write enable */
#define	OPCODE_RDSR		0x05	/* Read status register */
#define	OPCODE_WRSR		0x01	/* Write status register 1 byte */
#define	OPCODE_NORM_READ	0x03	/* Read data bytes (low frequency) */
#define	OPCODE_FAST_READ	0x0b	/* Read data bytes (high frequency) */
#define	OPCODE_PP		0x02	/* Page program (up to 256 bytes) */
#define	OPCODE_BE_4K		0x20	/* Erase 4KiB block */
#define	OPCODE_BE_32K		0x52	/* Erase 32KiB block */
#define	OPCODE_CHIP_ERASE	0xc7	/* Erase whole flash chip */
#define	OPCODE_SE		0xd8	/* Sector erase (usually 64KiB) */
#define	OPCODE_RDID		0x9f	/* Read JEDEC ID */

/* Status Register bits. */
#define	SR_WIP			1	/* Write in progress */
#define	SR_WEL			2	/* Write enable latch */
/* meaning of other SR_* bits may differ between vendors */
#define	SR_BP0			4	/* Block protect 0 */
#define	SR_BP1			8	/* Block protect 1 */
#define	SR_BP2			0x10	/* Block protect 2 */
#define	SR_SRWD			0x80	/* SR write protect */

/* Define max times to check status register before we give up. */
#define	MAX_READY_WAIT_COUNT	100000
#define	CMD_SIZE		4

#ifdef CONFIG_M25PXX_USE_FAST_READ
#define OPCODE_READ 	OPCODE_FAST_READ
#define FAST_READ_DUMMY_BYTE 1
#else
#define OPCODE_READ 	OPCODE_NORM_READ
#define FAST_READ_DUMMY_BYTE 0
#endif

#ifdef CONFIG_MTD_PARTITIONS
#define	mtd_has_partitions()	(1)
#else
#define	mtd_has_partitions()	(0)
#endif

/****************************************************************************/

struct m25p {
	struct spi_device	*spi;
	struct mutex		lock;
	struct mtd_info		mtd;
	unsigned		partitioned:1;
	u8			erase_opcode;
	u8			command[CMD_SIZE + FAST_READ_DUMMY_BYTE];
};

static inline struct m25p *mtd_to_m25p(struct mtd_info *mtd)
{
	return container_of(mtd, struct m25p, mtd);
}

/****************************************************************************/
void newchip_spi_read_id(struct spi_device *spi, unsigned char *id);
void newchip_spi_erase_chip(struct spi_device *spi);
void newchip_spi_erase(struct spi_device *spi, unsigned char erase_code, unsigned int offset);
void newchip_spi_read_block(struct spi_device *spi, unsigned int addr, unsigned int length, unsigned char * buf);
int newchip_spi_write_block(struct spi_device *spi, unsigned int offset, volatile unsigned int * addr, unsigned int length);

/****************************************************************************/

/*
 * MTD implementation
 */

/*
 * Erase an address range on the flash chip.  The address range may extend
 * one or more erase sectors.  Return an error is there is a problem erasing.
 */
static int m25p80_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	u32 addr,len;

	DEBUG(MTD_DEBUG_LEVEL2, "%s %s 0x%08x, len %lld\n",
			__func__, "at",
			(u32)instr->addr, instr->len);

	/* sanity checks */
	if (instr->addr + instr->len > flash->mtd.size)
		return -EINVAL;
	/*
	if ((instr->addr % mtd->erasesize) != 0
			|| (instr->len % mtd->erasesize) != 0) {
		return -EINVAL;
	}*/

	addr = instr->addr;
	len = instr->len;

	mutex_lock(&flash->lock);

	/* whole-chip erase? */
	if (len == flash->mtd.size) {
        newchip_spi_erase_chip(flash->spi);

	/* REVISIT in some cases we could speed up erasing large regions
	 * by using OPCODE_SE instead of OPCODE_BE_4K.  We may have set up
	 * to use "small sector erase", but that's not always optimal.
	 */

	/* "sector"-at-a-time erase */
	} else {
		while (len) {
			newchip_spi_erase(flash->spi, flash->erase_opcode, addr);

			addr += mtd->erasesize;
			len -= mtd->erasesize;
		}
	}

	mutex_unlock(&flash->lock);

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return 0;
}

static int newchip_m25p80_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	struct m25p *flash = mtd_to_m25p(mtd);

	DEBUG(MTD_DEBUG_LEVEL2, "%s %s 0x%08x, len %zd\n",
			__func__, "from",
			(u32)from, len);

	/* sanity checks */
	if (!len)
		return 0;

	if (from + len > flash->mtd.size)
		return -EINVAL;

	/* Byte count starts at zero. */
	if (retlen)
		*retlen = 0;


	mutex_lock(&flash->lock);

    newchip_spi_read_block(flash->spi, (unsigned int)from, len, buf);

	mutex_unlock(&flash->lock);

	if (retlen)
        *retlen = len;

	return 0;
}

static int newchip_m25p80_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	u32 page_offset, page_size;

	DEBUG(MTD_DEBUG_LEVEL2, "%s %s 0x%08x, len %zd\n",
			__func__, "to",
			(u32)to, len);

	if (retlen)
		*retlen = 0;

	/* sanity checks */
	if (!len)
		return(0);

	if (to + len > flash->mtd.size)
		return -EINVAL;

	mutex_lock(&flash->lock);

    /* what page do we start with? */
    page_offset = to % FLASH_PAGESIZE;

    /* do all the bytes fit onto one page? */
    if (page_offset + len <= FLASH_PAGESIZE) {
        newchip_spi_write_block(flash->spi, to, (unsigned int *)buf, len);
    } else {
	    u32 i;

	    /* the size of data remaining on the first page */
	    page_size = FLASH_PAGESIZE - page_offset;

        newchip_spi_write_block(flash->spi, to, (unsigned int *)buf, page_size);

	    /* write everything in PAGESIZE chunks */
	    for (i = page_size; i < len; i += page_size) {
		    page_size = len - i;
		    if (page_size > FLASH_PAGESIZE)
			    page_size = FLASH_PAGESIZE;

            newchip_spi_write_block(flash->spi, to+i, (unsigned int *)(buf+i), page_size);
	    }
    }


	if (retlen)
        *retlen = len;

	mutex_unlock(&flash->lock);

	return 0;
}

/****************************************************************************/

/*
 * SPI device driver setup and teardown
 */

struct flash_info {
	char		*name;

	/* JEDEC id zero means "no ID" (most older chips); otherwise it has
	 * a high byte of zero plus three data bytes: the manufacturer id,
	 * then a two byte device id.
	 */
	u32		jedec_id;
	u16             ext_id;

	/* The size listed here is what works with OPCODE_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	u16		n_sectors;

	u16		flags;
#define	SECT_4K		0x01		/* OPCODE_BE_4K works uniformly */
};


/* NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
static struct flash_info __devinitdata m25p_data [] = {

	/* Atmel -- some are (confusingly) marketed as "DataFlash" */
	{ "at25fs010",  0x1f6601, 0, 32 * 1024, 4, SECT_4K, },
	{ "at25fs040",  0x1f6604, 0, 64 * 1024, 8, SECT_4K, },

	{ "at25df041a", 0x1f4401, 0, 64 * 1024, 8, SECT_4K, },
	{ "at25df641",  0x1f4800, 0, 64 * 1024, 128, SECT_4K, },

	{ "at26f004",   0x1f0400, 0, 64 * 1024, 8, SECT_4K, },
	{ "at26df081a", 0x1f4501, 0, 64 * 1024, 16, SECT_4K, },
	{ "at26df161a", 0x1f4601, 0, 64 * 1024, 32, SECT_4K, },
	{ "at26df321",  0x1f4701, 0, 64 * 1024, 64, SECT_4K, },

	/* Spansion -- single (large) sector size only, at least
	 * for the chips listed here (without boot sectors).
	 */
	{ "s25sl004a", 0x010212, 0, 64 * 1024, 8, },
	{ "s25sl008a", 0x010213, 0, 64 * 1024, 16, },
	{ "s25sl016a", 0x010214, 0, 64 * 1024, 32, },
	{ "s25sl032a", 0x010215, 0, 64 * 1024, 64, },
	{ "s25sl064a", 0x010216, 0, 64 * 1024, 128, },
    { "s25sl12800", 0x012018, 0x0300, 256 * 1024, 64, },
	{ "s25sl12801", 0x012018, 0x0301, 64 * 1024, 256, },
    { "s25sl12800", 0x012018, 0x4D00, 256 * 1024, 64, },
    
	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf040b", 0xbf258d, 0, 64 * 1024, 8, SECT_4K, },
	{ "sst25vf080b", 0xbf258e, 0, 64 * 1024, 16, SECT_4K, },
	{ "sst25vf016b", 0xbf2541, 0, 64 * 1024, 32, SECT_4K, },
	{ "sst25vf032b", 0xbf254a, 0, 64 * 1024, 64, SECT_4K, },

	/* ST Microelectronics -- newer production may have feature updates */
	{ "m25p05",  0x202010,  0, 32 * 1024, 2, },
	{ "m25p10",  0x202011,  0, 32 * 1024, 4, },
	{ "m25p20",  0x202012,  0, 64 * 1024, 4, },
	{ "m25p40",  0x202013,  0, 64 * 1024, 8, },
	{ "m25p16",  0x202015,  0, 64 * 1024, 32, },
	{ "m25p32",  0x202016,  0, 64 * 1024, 64, },
	{ "m25p64",  0x202017,  0, 64 * 1024, 128, },
	{ "m25p128", 0x202018,  0, 256 * 1024, 64, },

	{ "m45pe80", 0x204014,  0, 64 * 1024, 16, },
	{ "m45pe16", 0x204015,  0, 64 * 1024, 32, },

	{ "m25pe80", 0x208014,  0, 64 * 1024, 16, },
	{ "m25pe16", 0x208015,  0, 64 * 1024, 32, SECT_4K, },

	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
	{ "w25x10", 0xef3011, 0, 64 * 1024, 2, SECT_4K, },
	{ "w25x20", 0xef3012, 0, 64 * 1024, 4, SECT_4K, },
	{ "w25x40", 0xef3013, 0, 64 * 1024, 8, SECT_4K, },
	{ "w25x80", 0xef3014, 0, 64 * 1024, 16, SECT_4K, },
	{ "w25x16", 0xef3015, 0, 64 * 1024, 32, SECT_4K, },
	{ "w25x32", 0xef3016, 0, 64 * 1024, 64, SECT_4K, },
	{ "w25x64", 0xef3017, 0, 64 * 1024, 128, SECT_4K, },

    /* GD */
    { "gd25q128b", 0xC84018, 0, 64 * 1024, 256, SECT_4K, },
};

static struct flash_info *__devinit jedec_probe(struct spi_device *spi)
{
	int			tmp;
	u8			id[5];
	u32			jedec;
	u16                     ext_jedec;
	struct flash_info	*info;

	/* JEDEC also defines an optional "extended device information"
	 * string for after vendor-specific data, after the three bytes
	 * we use here.  Supporting some chips might require using it.
	 */
 //   BITCLR( HwGPIOB->GPDAT, Hw22 );
	newchip_spi_read_id(spi, id);

	jedec = id[0];
	jedec = jedec << 8;
	jedec |= id[1];
	jedec = jedec << 8;
	jedec |= id[2];

	ext_jedec = id[3] << 8 | id[4];

	//printk("----------id[0] = 0x%X--------\n----------id[1] = 0x%X--------\n----------id[2] = 0x%X--------\n----------id[3] = 0x%X--------\n----------id[4] = 0x%X--------\n",id[0],id[1],id[2],id[3],id[4]);//gg

	for (tmp = 0, info = m25p_data;
			tmp < ARRAY_SIZE(m25p_data);
			tmp++, info++) {
		if (info->jedec_id == jedec) {
			if (info->ext_id != 0 && info->ext_id != ext_jedec){
				continue;
			}
			return info;
		}
	}
	dev_err(&spi->dev, "unrecognized JEDEC id %06x, %04x\n", jedec, ext_jedec);
	return NULL;
}


/*
 * board specific setup should have ensured the SPI clock used here
 * matches what the READ command supports, at least until this driver
 * understands FAST_READ (for clocks over 25 MHz).
 */
static int __devinit m25p_probe(struct spi_device *spi)
{
	struct flash_platform_data	*data;
	struct m25p			*flash;
	struct flash_info		*info;
	unsigned			i;

    printk("m25p_probe\r\n");
	/* Platform data helps sort out which chip type we have, as
	 * well as how this board partitions it.  If we don't have
	 * a chip ID, try the JEDEC id commands; they'll work for most
	 * newer chips, even if we don't recognize the particular chip.
	 */
	data = spi->dev.platform_data;
	spi->bits_per_word = 8;
	//printk("---------spi->max_speed_hz = %d---------\n---------spi->chip_select = 0x%X---------\n---------spi->mode = 0x%X---------\n---------spi->bits_per_word = 0x%X---------\n",spi->max_speed_hz,spi->chip_select,spi->mode,spi->bits_per_word);

	if (data && data->type) {
		for (i = 0, info = m25p_data;
				i < ARRAY_SIZE(m25p_data);
				i++, info++) {
			if (strcmp(data->type, info->name) == 0)
				break;
		}
		/* unrecognized chip? */
		if (i == ARRAY_SIZE(m25p_data)) {
			DEBUG(MTD_DEBUG_LEVEL0, "%s: unrecognized id %s\n", dev_name(&spi->dev), data->type);
			info = NULL;

		/* recognized; is that chip really what's there? */
		} else if (info->jedec_id) {
			
			struct flash_info	*chip= jedec_probe(spi);

			if (!chip || chip != info) {
				dev_warn(&spi->dev, "found %s, expected %s\n",
						chip ? chip->name : "UNKNOWN",
						info->name);
				info = chip;
			}
		}
	} else
    {
		info = jedec_probe(spi);
    }
	if (!info){
		return -ENODEV;
	}
	flash = kzalloc(sizeof *flash, GFP_KERNEL);
	if (!flash)
		return -ENOMEM;

	flash->spi = spi;
	mutex_init(&flash->lock);
	dev_set_drvdata(&spi->dev, flash);

	
	if (data && data->name)
		flash->mtd.name = data->name;
	else
		flash->mtd.name = dev_name(&spi->dev);

	flash->mtd.type = MTD_NORFLASH;
	flash->mtd.writesize = 1;
	flash->mtd.flags = MTD_CAP_NORFLASH;
	flash->mtd.size = info->sector_size * info->n_sectors;
	flash->mtd.erase = m25p80_erase;
	flash->mtd.read = newchip_m25p80_read;
	flash->mtd.write = newchip_m25p80_write;

	/* prefer "small sector" erase if possible */
	if (info->flags & SECT_4K) {
		flash->erase_opcode = OPCODE_BE_4K;
		flash->mtd.erasesize = 4096;
	} else {
		flash->erase_opcode = OPCODE_SE;
		flash->mtd.erasesize = info->sector_size;
	}

	dev_info(&spi->dev, "%s (%lld Kbytes)\n", info->name,
			flash->mtd.size / 1024);

	DEBUG(MTD_DEBUG_LEVEL2,
		"mtd .name = %s, .size = 0x%.8llx (%lluMiB) "
			".erasesize = 0x%.8x (%uKiB) .numeraseregions = %d\n",
		flash->mtd.name,
		flash->mtd.size, flash->mtd.size / (1024*1024),
		flash->mtd.erasesize, flash->mtd.erasesize / 1024,
		flash->mtd.numeraseregions);

	if (flash->mtd.numeraseregions)
		for (i = 0; i < flash->mtd.numeraseregions; i++)
			DEBUG(MTD_DEBUG_LEVEL2,
				"mtd.eraseregions[%d] = { .offset = 0x%.8llx, "
				".erasesize = 0x%.8x (%uKiB), "
				".numblocks = %d }\n",
				i, flash->mtd.eraseregions[i].offset,
				flash->mtd.eraseregions[i].erasesize,
				flash->mtd.eraseregions[i].erasesize / 1024,
				flash->mtd.eraseregions[i].numblocks);


	/* partitions should match sector boundaries; and it may be good to
	 * use readonly partitions for writeprotected sectors (BP2..BP0).
	 */
	if (mtd_has_partitions()) {
		struct mtd_partition	*parts = NULL;
		int			nr_parts = 0;

#ifdef CONFIG_MTD_CMDLINE_PARTS
		static const char *part_probes[] = { "cmdlinepart", NULL, };

		nr_parts = parse_mtd_partitions(&flash->mtd,
				part_probes, &parts, 0);
#endif

		if (nr_parts <= 0 && data && data->parts) {
			parts = data->parts;
			nr_parts = data->nr_parts;
		}

		if (nr_parts > 0) {
			for (i = 0; i < nr_parts; i++) {
				DEBUG(MTD_DEBUG_LEVEL2, "partitions[%d] = "
					"{.name = %s, .offset = 0x%.8llx, "
						".size = 0x%.8llx (%lluKiB) }\n",
					i, parts[i].name,
					parts[i].offset,
					parts[i].size,
					parts[i].size / 1024);
			}
			flash->partitioned = 1;
			return add_mtd_partitions(&flash->mtd, parts, nr_parts);
		}
	} else if (data->nr_parts)
		dev_warn(&spi->dev, "ignoring %d default partitions on %s\n",
				data->nr_parts, data->name);

	return add_mtd_device(&flash->mtd) == 1 ? -ENODEV : 0;
}


static int __devexit m25p_remove(struct spi_device *spi)
{
	struct m25p	*flash = dev_get_drvdata(&spi->dev);
	int		status;

	/* Clean up MTD stuff. */
	if (mtd_has_partitions() && flash->partitioned)
		status = del_mtd_partitions(&flash->mtd);
	else
		status = del_mtd_device(&flash->mtd);
	if (status == 0)
		kfree(flash);
	return 0;
}


static struct spi_driver m25p80_driver = {
	.driver = {
		.name	= "m25p128",
		.bus	= &spi_bus_type,
		.owner	= THIS_MODULE,
	},
	.probe	= m25p_probe,
	.remove	= __devexit_p(m25p_remove),

	/* REVISIT: many of these chips have deep power-down modes, which
	 * should clearly be entered on suspend() to minimize power use.
	 * And also when they're otherwise idle...
	 */
};


static int m25p80_init(void)
{
	printk("--------m25p128_init--------\n");


	
	return spi_register_driver(&m25p80_driver);
}


static void m25p80_exit(void)
{
//	printk("-----m25p128_exit--------\n");
	spi_unregister_driver(&m25p80_driver);
}


module_init(m25p80_init);
module_exit(m25p80_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mike Lavender");
MODULE_DESCRIPTION("MTD SPI driver for ST M25Pxx flash chips");
