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
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	//struct newchip_spi_slave *ds = to_newchip_spi(slave);

	/* Disable the SPI hardware */
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
		void *din, unsigned long flags)
{

	debug("%s bitlen:%i flags:%lx\n", __func__,
			bitlen, flags);
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
