#define DEBUG
#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>
#include "newchip_spi.h"

void spi_init()
{
	/* do nothing */
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
			unsigned int max_hz, unsigned int mode)
{
	struct newchip_spi_slave	*ds;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	ds = malloc(sizeof(*ds));
	if (!ds)
		return NULL;

	ds->slave.bus = bus;
	ds->slave.cs = cs;
	ds->regs = (struct bspi_regs *)CONFIG_SYS_SPI_BASE;
	ds->freq = max_hz;
	debug("spi_setup_slave @%x\n",CONFIG_SYS_SPI_BASE);
	return &ds->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct newchip_spi_slave *bspi = to_newchip_spi(slave);

	free(bspi);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct newchip_spi_slave *bspi = to_newchip_spi(slave);
	unsigned int scalar;
	volatile unsigned int address, wr_data, rd_data;

	debug("spi_claim_bus\n");
	/////////////////////////
	// CTRL1 register
	/////////////////////////
	// Bit14(wp_pad_o)  : wp pin
	// Bit13(ass)       : 1-cs pin generated automatically 
	// Bit11(lsb)       : 0-MSB first, 1-LSB first
	// Bit10(mosi_neg)  : 0-send data on sclk posedge, 1-send data on sclk negedge
	// Bit9 (miso_neg)  : 0-receive data on sclk posedge, 1-receive data on sclk negedge
	//
	// 0010_0100_0000_0000
	bspi->regs->ctrl1 = 0x2400; 


	/////////////////////////
	// CTRL2 register
	/////////////////////////
	// Bit[23:16](spi_ss)   :  setting the spi_ss register to zero will choose the connection slave devices
	// Bit[15:0](sclk_div)  :  16'd0 => sclk = 2x system clock
	//                         16'd1 => sclk = 4x system clock
	//                         16'd2 => sclk = 6x system clock
	//                         ....
	//                         16'd65535 => sclk = 131072 x system clock
	//
	// 
	bspi->regs->ctrl2 = 0x10009;       // ASIC: 10Mhz SCLK (200Mhz/20)



	debug("test read flash id\n");
	bspi->regs->Tx1 = 0x00009000;       
	/////////////////////////
	// CTRL0 register
	/////////////////////////
	// Bit[6:0] (char_len) : Char_len = 7'b0   => 128bits
	//                       Char_len = 7'd127 => 127bits
	bspi->regs->ctrl0 = 48;            // TX: 32bits & RX: 16bits

	/////////////////////////
	// SPI_GO register
	/////////////////////////
	// Start 
	bspi->regs->spi_go = 0x1;            // TX: 32bits & RX: 16bits

	// Status check
	rd_data = bspi->regs->spi_go;
	while(rd_data&0x1) {
		rd_data = bspi->regs->spi_go;
	}
	rd_data = bspi->regs->Rx0;
	rd_data = rd_data & 0xffff;
	debug("flash id %x\n",rd_data); 

	debug("test cmd 9f\n");	
	bspi->regs->Tx0 = 0x9F;
	bspi->regs->ctrl0 = 48;		     //TX: 8 & RX: 40
	bspi->regs->spi_go = 0x1;            // TX: 8bits & RX: 40bits
	rd_data = bspi->regs->spi_go;
	while(rd_data&0x1) {
		rd_data = bspi->regs->spi_go;
	}
	debug("cmd 9f return %x %x\n",bspi->regs->Rx0,bspi->regs->Rx1); 

	debug("test cmd 9f emulate spi_xfer\n");
	bspi->regs->Tx0 = 0x9F;
	bspi->regs->ctrl0 = 8;		     //TX: 8 & RX: 40
	bspi->regs->spi_go = 0x1;            // TX: 8bits & RX: 40bits
	rd_data = bspi->regs->spi_go;
	while(rd_data&0x1) {
		rd_data = bspi->regs->spi_go;
	}
	debug("cmd 9f return %x %x\n",bspi->regs->Rx0,bspi->regs->Rx1); 
	bspi->regs->Tx0 = 0;
	bspi->regs->ctrl0 = 8;		     //TX: 8 & RX: 40
	bspi->regs->spi_go = 0x1;            // TX: 8bits & RX: 40bits
	rd_data = bspi->regs->spi_go;
	while(rd_data&0x1) {
		rd_data = bspi->regs->spi_go;
	}
	debug("cmd 9f return %x %x\n",bspi->regs->Rx0,bspi->regs->Rx1); 
	bspi->regs->Tx0 = 0;
	bspi->regs->ctrl0 = 8;		     //TX: 8 & RX: 40
	bspi->regs->spi_go = 0x1;            // TX: 8bits & RX: 40bits
	rd_data = bspi->regs->spi_go;
	while(rd_data&0x1) {
		rd_data = bspi->regs->spi_go;
	}
	debug("cmd 9f return %x %x\n",bspi->regs->Rx0,bspi->regs->Rx1); 
	bspi->regs->Tx0 = 0;
	bspi->regs->ctrl0 = 8;		     //TX: 8 & RX: 40
	bspi->regs->spi_go = 0x1;            // TX: 8bits & RX: 40bits
	rd_data = bspi->regs->spi_go;
	while(rd_data&0x1) {
		rd_data = bspi->regs->spi_go;
	}
	debug("cmd 9f return %x %x\n",bspi->regs->Rx0,bspi->regs->Rx1); 
	bspi->regs->Tx0 = 0;
	bspi->regs->ctrl0 = 8;		     //TX: 8 & RX: 40
	bspi->regs->spi_go = 0x1;            // TX: 8bits & RX: 40bits
	rd_data = bspi->regs->spi_go;
	while(rd_data&0x1) {
		rd_data = bspi->regs->spi_go;
	}
	debug("cmd 9f return %x %x\n",bspi->regs->Rx0,bspi->regs->Rx1); 
	bspi->regs->Tx0 = 0;
	bspi->regs->ctrl0 = 8;		     //TX: 8 & RX: 40
	bspi->regs->spi_go = 0x1;            // TX: 8bits & RX: 40bits
	rd_data = bspi->regs->spi_go;
	while(rd_data&0x1) {
		rd_data = bspi->regs->spi_go;
	}
	debug("cmd 9f return %x %x\n",bspi->regs->Rx0,bspi->regs->Rx1); 
	
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	//struct newchip_spi_slave *ds = to_newchip_spi(slave);

	/* Disable the SPI hardware */
}

#ifndef CONFIG_BSPI_IDLE_VAL
# define CONFIG_BSPI_IDLE_VAL 0x00
#endif

int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
		void *din, unsigned long flags)
{
	struct newchip_spi_slave *bspi = to_newchip_spi(slave);
	/* assume spi core configured to do 8 bit transfers */
	uint bytes = bitlen / 8;
	const uchar *txp = dout;
	uchar *rxp = din;
	volatile unsigned int go_ready;

	debug("%s: bus:%i cs:%i bitlen:%i bytes:%i flags:%lx\n", __func__,
			slave->bus, slave->cs, bitlen, bytes, flags);
	if (bitlen == 0)
		goto done;

	if (bitlen % 8) {
		flags |= SPI_XFER_END;
		goto done;
	}

	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	while (bytes--) {
		uchar d = txp ? *txp++ : CONFIG_BSPI_IDLE_VAL;
		debug("%s: tx:%x ", __func__, d);
		bspi->regs->Tx0 = d;
		bspi->regs->ctrl0 = 8;
		bspi->regs->spi_go = 0x1; 
		go_ready = bspi->regs->spi_go;
		while(go_ready&0x1) {
			go_ready = bspi->regs->spi_go;
		}
		d = bspi->regs->Rx0&0xff;
		if (rxp)
			*rxp++ = d;
		debug("rx:%x\n", d);
	}
done:
	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);

	return 0;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs == 0;
}

void spi_cs_activate(struct spi_slave *slave)
{
	/* do nothing */
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	/* do nothing */
}
