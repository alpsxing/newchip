/*
 * Driver for Newchip Controllers
 *
 * Copyright (C)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>

#include <asm/io.h>
#include <mach/hardware.h>
#include "newchip_spi.h"

/*
 * The core SPI transfer engine just talks to a register bank to set up
 * DMA transfers; transfer queue progress is driven by IRQs.  The clock
 * framework provides the base clock, subdivided for each spi_device.
 *
 * Newer controllers, marked with "new_1" flag, have:
 *  - CR.LASTXFER
 *  - SPI_MR.DIV32 may become FDIV or must-be-zero (here: always zero)
 *  - SPI_SR.TXEMPTY, SPI_SR.NSSR (and corresponding irqs)
 *  - SPI_CSRx.CSAAT
 *  - SPI_CSRx.SBCR allows faster clocking
 */
struct newchip_spi {
	void __iomem		*regs;
	struct platform_device	*pdev;
};

#define BUFFER_SIZE		PAGE_SIZE
#define INVALID_DMA_ADDRESS	0xffffffff

static void spi_wait_ready(struct newchip_spi *spi)
{
    unsigned int rd_data;
	
	// Status check
	do{
	     rd_data = spi_readl(spi, GO);
	}while(rd_data&0x1); 
}

static void spi_write_en(struct newchip_spi *spi)
{
    spi_writel(spi, TX0, WRITE_EN_CMD);
    spi_writel(spi, CTRL0, CMD_LEN);

	//start transaction
    spi_writel(spi, GO, 0x1);

	//wait transaction done
	spi_wait_ready(spi);
}


static void spi_write_disable(struct newchip_spi *spi)
{
    spi_writel(spi, TX0, WRITE_DIS_CMD);
    spi_writel(spi, CTRL0, CMD_LEN);

	//start transaction
    spi_writel(spi, GO, 0x1);

	//wait transaction done
	spi_wait_ready(spi);
}

static int spi_write_status(struct newchip_spi *spi)
{
    unsigned int rd_data; 
    unsigned int write_protect, write_in_progress;
 
	do {
	    //read Status 
        spi_writel(spi, TX0, READ_SR_CMD<<8);
	    
        spi_writel(spi, CTRL0, CMD_LEN + 8);
	    
	    //start transaction
        spi_writel(spi, GO, 0x1);
	    
	    //wait transaction done
	    spi_wait_ready(spi);
	    
	    //check status
	    rd_data = spi_readl(spi, RX0);
	    
	    write_protect = (rd_data >> 7) & 0x00000001;
	    write_in_progress = rd_data & 0x00000001;

	} while(write_in_progress & (write_protect == 0));

	//TRACE(KERN_ERROR, "Flash Write Status Check finish.\n");

	return write_protect;
}

void newchip_spi_read_id(struct spi_device *spi, unsigned char *id)
{
    unsigned int rd_data; 
    struct newchip_spi	*ns;

    ns = spi_master_get_devdata(spi->master);
    printk("newchip_spi_read_id 0x%08lx\n", (unsigned long)ns->regs);

    spi_writel(ns, TX1, READ_ID_CMD<<8);
    spi_writel(ns, CTRL0, 48);
    spi_writel(ns, GO, 0x1);
	spi_wait_ready(ns);

    rd_data = spi_readl(ns, RX1);
	id[0] = rd_data&0xff;	
	rd_data = spi_readl(ns, RX0);
	id[1] = (rd_data & 0xff000000) >> 24;	
	id[2] = (rd_data & 0xff0000) >> 16;
	id[3] = (rd_data & 0xff00) >> 8;
	id[4] = (rd_data & 0xff);
	
	return;
}

void newchip_spi_erase_chip(struct spi_device *spi)
{
    struct newchip_spi	*ns;

    ns = spi_master_get_devdata(spi->master);

    spi_write_en(ns);

    spi_writel(ns, TX0, BULK_ERASE_CMD);
    spi_writel(ns, CTRL0, 8);
    spi_writel(ns, GO, 0x1);
	spi_wait_ready(ns);
    spi_write_status(ns);

	return;
}

void newchip_spi_erase(struct spi_device *spi, unsigned char erase_code, unsigned int offset)
{
    struct newchip_spi	*ns;

    printk("newchip_spi_erase 0x%08x\r\n", offset);

    ns = spi_master_get_devdata(spi->master);

    spi_write_en(ns);

    spi_writel(ns, TX0, (erase_code << (ADDR_LEN)) | offset);
    spi_writel(ns, CTRL0, 32);
    spi_writel(ns, GO, 0x1);
	spi_wait_ready(ns);
    spi_write_status(ns);

	return;
}

static void spi_read_bytes(struct spi_device *spi, unsigned int addr, unsigned int byte_cnt, unsigned char *buf)
{
    unsigned int rd_data;
    struct newchip_spi	*ns;

    ns = spi_master_get_devdata(spi->master);
    
    spi_writel(ns, TX1, (READ_BYTES_CMD<<(ADDR_LEN)) | addr);
    spi_writel(ns, CTRL0, (CMD_LEN + ADDR_LEN) + 8*byte_cnt);

	//start transaction
    spi_writel(ns, GO, 0x1);

	//wait transaction done
	spi_wait_ready(ns);
 
	rd_data = spi_readl(ns, RX0);

	buf[0] = (rd_data & 0xff000000) >> 24;	
	buf[1] = (rd_data & 0xff0000) >> 16;
	buf[2] = (rd_data & 0xff00) >> 8;
	buf[3] = (rd_data & 0xff);

	
}

void newchip_spi_read_block(struct spi_device *spi, unsigned int addr, unsigned int length, unsigned char * buf)
{
	unsigned char temp[4]; //128 bit buffer
	unsigned int  cycle_cnt;
	unsigned int  tail_bcnt;
	unsigned int  flash_addr;

	int i,j;

    printk("newchip_spi_read_block 0x%08x 0x%08x 0x%08x\r\n", (unsigned int)buf, (unsigned int)addr, length);

	flash_addr= addr & ((1<<ADDR_LEN)-1);

	cycle_cnt  = length>>2;
	tail_bcnt  = length%4;

	if (tail_bcnt != 0)
		cycle_cnt = cycle_cnt + 1;

	for (i=0; i<cycle_cnt; i++)
	{ 
		//read 4 byte each time
		spi_read_bytes(spi, flash_addr, 4, temp);
		//printf("RF %x\n", *((unsigned int * )temp));

		for (j=0; j<4; j++)
		{
			*buf = temp[j];
			buf++;
		}  

		flash_addr +=4;
	}
}

static int spi_write_bytes(struct spi_device *spi, unsigned int addr, unsigned int byte_cnt, unsigned int wr_data)
{
	unsigned int status = 0;
    struct newchip_spi	*ns;

    ns = spi_master_get_devdata(spi->master);
    spi_write_en(ns);

    spi_writel(ns, TX1, (PAGE_PROG_CMD <<ADDR_LEN) | addr);
    spi_writel(ns, TX0, ((wr_data & 0xff)<<24) | ((wr_data & 0xff00)<<8) | ((wr_data & 0xff0000)>>8) | ((wr_data & 0xff000000)>>24));
    spi_writel(ns, CTRL0, (CMD_LEN + ADDR_LEN) + 8*byte_cnt);

	//start transaction
    spi_writel(ns, GO, 0x1);

	//wait transaction done
	spi_wait_ready(ns);

	//wait flash write finish & the flash write is auto disabled
	status = spi_write_status(ns);
    spi_write_en(ns);
 	
	return status;
}

int newchip_spi_write_block(struct spi_device *spi, unsigned int offset, volatile unsigned int * addr, unsigned int length)
{
	unsigned int  temp; 
	unsigned int  cycle_cnt;
	unsigned int  tail_bcnt;
	unsigned int  status = 0;
	int i;

    printk("newchip_spi_write_block 0x%08x 0x%08x 0x%08x\r\n", offset, (unsigned int)addr, length);

	cycle_cnt  = length>>2;
	tail_bcnt  = length%4;

	if (tail_bcnt != 0)
		cycle_cnt = cycle_cnt + 1;


	for (i=0; i<cycle_cnt; i++)
	{ 
		temp = *(addr + i);

		//TRACE(KERN_DEBUG, "Flash Burn at word %d : %x\n", i, temp);

		//write 4 byte each time
		status = spi_write_bytes(spi, offset+4*i, 4, temp);

		if(status) break;
	}

	return status;
}


static int newchip_spi_setup(struct spi_device *spi)
{
	return 0;
}

static int newchip_spi_transfer(struct spi_device *spi, struct spi_message *msg)
{
	return 0;
}

static void newchip_spi_cleanup(struct spi_device *spi)
{
    return;
}

/*-------------------------------------------------------------------------*/

static int __init newchip_spi_probe(struct platform_device *pdev)
{
	struct resource		*regs;
	int			ret;
	struct spi_master	*master;
	struct newchip_spi	*ns;
    int clk_div_rate;

    printk("Newchip SPI Probe");
	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs)
		return -ENXIO;

	/* setup spi core then atmel-specific driver state */
	ret = -ENOMEM;
	master = spi_alloc_master(&pdev->dev, sizeof *ns);
	if (!master)
		goto out_free;

	master->bus_num = pdev->id;
	master->num_chipselect = 1;
	master->setup = newchip_spi_setup;
	master->transfer = newchip_spi_transfer;
	master->cleanup = newchip_spi_cleanup;
	platform_set_drvdata(pdev, master);

	ns = spi_master_get_devdata(master);
	ns->pdev = pdev;
    //ns->regs = (bspi_regs *)(regs->start);

	ns->regs = (bspi_regs *)ioremap_nocache(regs->start, (regs->end - regs->start) + 1);
    if(!ns->regs)
    {
        printk("ioremap failed.");
        goto out_unmap_regs;
    }

	/* Initialize the hardware */
    spi_writel(ns, CTRL1, WP_POS_EN | ASS_EN | MOSI_NEG_EN);
    clk_div_rate    = ((CFG_APB_CLOCK/CFG_SPI_CLOCK)>>1)-1;
    //use cs0 select slave 
    spi_writel(ns, CTRL2, clk_div_rate | CS0_EN);
    
	/* go! */
	dev_info(&pdev->dev, "Newchip SPI Controller at 0x%08lx 0x%08lx\n",
			(unsigned long)regs->start, (unsigned long)ns->regs);

	ret = spi_register_master(master);
	if (ret)
		goto out_reset_hw;

	return 0;

out_reset_hw:
    spi_writel(ns, CTRL1, 0);
    spi_writel(ns, CTRL2, 0);

out_unmap_regs:
	iounmap(ns->regs);
out_free:
	spi_master_put(master);
	return ret;
}

static int __exit newchip_spi_remove(struct platform_device *pdev)
{
	struct spi_master	*master = platform_get_drvdata(pdev);

	spi_unregister_master(master);

	return 0;
}

#define	newchip_spi_suspend	NULL
#define	newchip_spi_resume	NULL

static struct platform_driver newchip_spi_driver = {
	.driver		= {
		.name	= "newchip_spi",
		.owner	= THIS_MODULE,
	},
	.suspend	= newchip_spi_suspend,
	.resume		= newchip_spi_resume,
	.remove		= __exit_p(newchip_spi_remove),
};

static int __init newchip_spi_init(void)
{
        printk("Newchip SPI Init");
	return platform_driver_probe(&newchip_spi_driver, newchip_spi_probe);
}
module_init(newchip_spi_init);

static void __exit newchip_spi_exit(void)
{
	platform_driver_unregister(&newchip_spi_driver);
}
module_exit(newchip_spi_exit);

MODULE_DESCRIPTION("Newchip SPI Controller driver");
MODULE_AUTHOR("XXX");
MODULE_LICENSE("GPL");
