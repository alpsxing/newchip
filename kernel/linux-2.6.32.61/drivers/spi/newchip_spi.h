/*
 * Register definitions for Atmel Serial Peripheral Interface (SPI)
 *
 * Copyright (C) 2006 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __NEWCHIP_SPI_H__
#define __NEWCHIP_SPI_H__


#define WP_PAD_HI   0x4000  /*WP pin set 1 */
#define ASS_EN      0x2000  /*slave select auto generate enable */
#define LSB_MODE    0x1000  /*set LSB first mode */
#define MOSI_NEG_EN 0x0400  /*send data use sclk negedge */
#define MISO_NEG_EN 0x0300  /*receive data use sclk negedge */
#define WP_POS_EN   0x4000  /*Write protect bit set to 1 allow software write flash*/
#define CS0_EN      0x10000 /*controller use cs0 select slave.*/

/*TODO begin: add support on ASIC later*/
#define EXP_SLAVE_ID     0x0117 /*spansion SPI device ID support in FPGA*/
#define READ_ID_CMD      0x9f
#define WRITE_EN_CMD     0x06
#define WRITE_DIS_CMD    0x04
#define READ_SR_CMD      0x05
#define WRITE_SR_CMD     0x01
#define PAGE_PROG_CMD    0x02
#define READ_BYTES_CMD   0x03 
#define BULK_ERASE_CMD   0xc7
#define CMD_LEN          8    /*bit*/ 
#define ID_LEN           16   /*bit*/
#define ADDR_LEN         24   /*bit*/

#define spi_readl(port,reg) \
	__raw_readl((port)->regs + SPI_##reg)
#define spi_writel(port,reg,value) \
	__raw_writel((value), (port)->regs + SPI_##reg)

#define SPI_GO      0x0000
#define SPI_CTRL0   0x0004
#define SPI_CTRL1   0x0008
#define SPI_CTRL2   0x000c
#define SPI_RX0     0x0010
#define SPI_RX1     0x0014
#define SPI_RX2     0x0018
#define SPI_RX3     0x001c
#define SPI_TX0     0x0020
#define SPI_TX1     0x0024
#define SPI_TX2     0x0028
#define SPI_TX3     0x002c

#endif /* __ATMEL_SPI_H__ */
