/*
 * (C) Copyright 2002
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * SPI Read/Write Utilities
 */

#include <common.h>
#include <command.h>
#include <spi.h>

/*-----------------------------------------------------------------------
 * Definitions
 */

#ifndef MAX_SPI_BYTES
#   define MAX_SPI_BYTES 32	/* Maximum number of bytes we can handle */
#endif

#ifndef CONFIG_DEFAULT_SPI_BUS
#   define CONFIG_DEFAULT_SPI_BUS	0
#endif
#ifndef CONFIG_DEFAULT_SPI_MODE
#   define CONFIG_DEFAULT_SPI_MODE	SPI_MODE_0
#endif

/*
 * Values from last command.
 */
static unsigned int	bus;
static unsigned int	cs;
static unsigned int	mode;
static int   		bitlen;
static uchar 		dout[MAX_SPI_BYTES];
static uchar 		din[MAX_SPI_BYTES];

/*
 * SPI read/write
 *
 * Syntax:
 *   spi {dev} {num_bits} {dout}
 *     {dev} is the device number for controlling chip select (see TBD)
 *     {num_bits} is the number of bits to send & receive (base 10)
 *     {dout} is a hexadecimal string of data to send
 * The command prints out the hexadecimal string received via SPI.
 */

#ifdef CONFIG_NEWCHIP_SPI
#include <newchip_spi.h>
#define spi_port ((UR0431A_SPI_t)SPI_0_BASE)
static int newchip_spi_init(void)
{
        int clk_div_rate;
	//set work mode
        spi_port->ctrl1 = WP_POS_EN | ASS_EN | MOSI_NEG_EN;

	//set spi clock
	clk_div_rate    = ((CFG_APB_CLOCK/CFG_SPI_CLOCK)>>1)-1;
   
        //use cs0 select slave 
        spi_port->ctrl2 = clk_div_rate | CS0_EN;

	return 0;	
}

static void spi_wait_ready(void)
{
        unsigned int rd_data;
	
	// Status check
	do{
	     rd_data = spi_port->spi_go;
	}while(rd_data&0x1); 
}

static int newchip_spi_xfer(struct spi_slave *slave, unsigned int blen, u8 *out,
	u8 *in, unsigned long flags)
{
	unsigned int rd_data[4]; 
	unsigned int wr_data[4];

	printf("bitlen = %d\n",blen);
	if(blen > 128)
	{
		printf("bitlen = %d not implemented!\n",blen);
		return -1;
	}

	wr_data[3] = out[0] << 24 | out[1] << 16 | out[2] << 8 | out[3];
	wr_data[2] = out[4] << 24 | out[5] << 16 | out[6] << 8 | out[7];
	wr_data[1] = out[8] << 24 | out[9] << 16 | out[10] << 8 | out[11];
	wr_data[0] = out[12] << 24 | out[13] << 16 | out[14] << 8 | out[15];
	
	
	spi_port->Tx0 = wr_data[0]; 
	spi_port->Tx1 = wr_data[1]; 
	spi_port->Tx2 = wr_data[2]; 
	spi_port->Tx3 = wr_data[3]; 
	spi_port->ctrl0 = 0;		     //128bit
	spi_port->spi_go = 0x1;     
	spi_wait_ready();

	rd_data[0] = spi_port->Rx0;
	rd_data[1] = spi_port->Rx1;
	rd_data[2] = spi_port->Rx2;
	rd_data[3] = spi_port->Rx3;

	in[0] = (rd_data[3] & 0xff000000) >> 24;	
	in[1] = (rd_data[3] & 0xff0000) >> 16;
	in[2] = (rd_data[3] & 0xff00) >> 8;
	in[3] = (rd_data[3] & 0xff);
	in[4] = (rd_data[2] & 0xff000000) >> 24;	
	in[5] = (rd_data[2] & 0xff0000) >> 16;
	in[6] = (rd_data[2] & 0xff00) >> 8;
	in[7] = (rd_data[2] & 0xff);
	in[8] = (rd_data[1] & 0xff000000) >> 24;	
	in[9] = (rd_data[1] & 0xff0000) >> 16;
	in[10] = (rd_data[1] & 0xff00) >> 8;
	in[11] = (rd_data[1] & 0xff);
	in[12] = (rd_data[0] & 0xff000000) >> 24;	
	in[13] = (rd_data[0] & 0xff0000) >> 16;
	in[14] = (rd_data[0] & 0xff00) >> 8;
	in[15] = (rd_data[0] & 0xff);

	return 0;
}

int do_spi (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct spi_slave *slave;
	char  *cp = 0;
	uchar tmp;
	int   j;
	int   rcode = 0;

	/*
	 * We use the last specified parameters, unless new ones are
	 * entered.
	 */

	if ((flag & CMD_FLAG_REPEAT) == 0)
	{
		if (argc >= 2) {
			mode = CONFIG_DEFAULT_SPI_MODE;
			bus = simple_strtoul(argv[1], &cp, 10);
			if (*cp == ':') {
				cs = simple_strtoul(cp+1, &cp, 10);
			} else {
				cs = bus;
				bus = CONFIG_DEFAULT_SPI_BUS;
			}
			if (*cp == '.')
				mode = simple_strtoul(cp+1, NULL, 10);
		}
		if (argc >= 3)
			bitlen = simple_strtoul(argv[2], NULL, 10);
		if (argc >= 4) {
			cp = argv[3];
			for(j = 0; *cp; j++, cp++) {
				tmp = *cp - '0';
				if(tmp > 9)
					tmp -= ('A' - '0') - 10;
				if(tmp > 15)
					tmp -= ('a' - 'A');
				if(tmp > 15) {
					printf("Hex conversion error on %c\n", *cp);
					return 1;
				}
				if((j % 2) == 0)
					dout[j / 2] = (tmp << 4);
				else
					dout[j / 2] |= tmp;
			}
		}
	}

	if ((bitlen < 0) || (bitlen >  (MAX_SPI_BYTES * 8))) {
		printf("Invalid bitlen %d\n", bitlen);
		return 1;
	}
	printf("newchip_spi_init\n");
	newchip_spi_init();
	if(newchip_spi_xfer(slave, bitlen, dout, din,
				SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
		printf("Error during SPI transaction\n");
		rcode = 1;
	} else {
		for(j = 0; j < ((bitlen + 7) / 8); j++) {
			printf("%02X", din[j]);
		}
		printf("\n");
	}

	return rcode;
}

#else
int do_spi (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct spi_slave *slave;
	char  *cp = 0;
	uchar tmp;
	int   j;
	int   rcode = 0;

	/*
	 * We use the last specified parameters, unless new ones are
	 * entered.
	 */

	if ((flag & CMD_FLAG_REPEAT) == 0)
	{
		if (argc >= 2) {
			mode = CONFIG_DEFAULT_SPI_MODE;
			bus = simple_strtoul(argv[1], &cp, 10);
			if (*cp == ':') {
				cs = simple_strtoul(cp+1, &cp, 10);
			} else {
				cs = bus;
				bus = CONFIG_DEFAULT_SPI_BUS;
			}
			if (*cp == '.')
				mode = simple_strtoul(cp+1, NULL, 10);
		}
		if (argc >= 3)
			bitlen = simple_strtoul(argv[2], NULL, 10);
		if (argc >= 4) {
			cp = argv[3];
			for(j = 0; *cp; j++, cp++) {
				tmp = *cp - '0';
				if(tmp > 9)
					tmp -= ('A' - '0') - 10;
				if(tmp > 15)
					tmp -= ('a' - 'A');
				if(tmp > 15) {
					printf("Hex conversion error on %c\n", *cp);
					return 1;
				}
				if((j % 2) == 0)
					dout[j / 2] = (tmp << 4);
				else
					dout[j / 2] |= tmp;
			}
		}
	}

	if ((bitlen < 0) || (bitlen >  (MAX_SPI_BYTES * 8))) {
		printf("Invalid bitlen %d\n", bitlen);
		return 1;
	}

	slave = spi_setup_slave(bus, cs, 1000000, mode);
	if (!slave) {
		printf("Invalid device %d:%d\n", bus, cs);
		return 1;
	}

	spi_claim_bus(slave);
	if(spi_xfer(slave, bitlen, dout, din,
				SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
		printf("Error during SPI transaction\n");
		rcode = 1;
	} else {
		for(j = 0; j < ((bitlen + 7) / 8); j++) {
			printf("%02X", din[j]);
		}
		printf("\n");
	}
	spi_release_bus(slave);
	spi_free_slave(slave);

	return rcode;
}
#endif
/***************************************************/

U_BOOT_CMD(
	sspi,	5,	1,	do_spi,
	"SPI utility command",
	"[<bus>:]<cs>[.<mode>] <bit_len> <dout> - Send and receive bits\n"
	"<bus>     - Identifies the SPI bus\n"
	"<cs>      - Identifies the chip select\n"
	"<mode>    - Identifies the SPI mode to use\n"
	"<bit_len> - Number of bits to send (base 10)\n"
	"<dout>    - Hexadecimal string that gets sent"
);
