/*
 * SPI flash interface
 *
 * Copyright (C) 2008 Atmel Corporation
 * Copyright (C) 2010 Reinhard Meyer, EMK Elektronik
 *
 * Licensed under the GPL-2 or later.
 */
#define DEBUG
#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <spi_flash.h>
#include <watchdog.h>

#include "spi_flash_internal.h"

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

static int spi_read_id(u8 *id)
{
        unsigned int rd_data; 

        spi_port->Tx1 = (CMD_READ_ID<<8); 
	spi_port->ctrl0 = 48;		     //TX: 8 & RX: 40
	spi_port->spi_go = 0x1;     
	spi_wait_ready();

	id[0] = spi_port->Rx1&0xff;	
	rd_data = spi_port->Rx0;
	id[1] = (rd_data & 0xff000000) >> 24;	
	id[2] = (rd_data & 0xff0000) >> 16;
	id[3] = (rd_data & 0xff00) >> 8;
	id[4] = (rd_data & 0xff);
	
	return 0;
}

void spi_write_en(void)
{
       spi_port->Tx0   =  WRITE_EN_CMD;  
       spi_port->ctrl0 =  CMD_LEN;

	//start transaction
	spi_port->spi_go = 0x1;     

	//wait transaction done
	spi_wait_ready();

	//TRACE(KERN_ERROR, "Flash Write Enable.\n");
}


void spi_write_disable(void)
{
       spi_port->Tx0   =  WRITE_DIS_CMD;  
       spi_port->ctrl0 =  CMD_LEN;

	//start transaction
	spi_port->spi_go = 0x1;     

	//wait transaction done
	spi_wait_ready();

	//TRACE(KERN_ERROR, "Flash Write Enable.\n");
}

int spi_write_status(void)
{
        unsigned int rd_data; 
        unsigned int write_protect, write_in_progress;
 
	do {
	    //read Status 
            spi_port->Tx0   = (READ_SR_CMD<<8);
	    
	    spi_port->ctrl0 = CMD_LEN + 8;
	    
	    //start transaction
	    spi_port->spi_go = 0x1;     
	    
	    //wait transaction done
	    spi_wait_ready();
	    
	    //check status
	    rd_data = spi_port->Rx0;
	    
	    write_protect = (rd_data >> 7) & 0x00000001;
	    write_in_progress = rd_data & 0x00000001;

	} while(write_in_progress & (write_protect == 0));

	//TRACE(KERN_ERROR, "Flash Write Status Check finish.\n");

	return write_protect;
	
}


void spi_read_bytes(unsigned int addr, unsigned int byte_cnt, unsigned char *buf)
{
        unsigned int rd_data;
    
	spi_port->Tx1   = (READ_BYTES_CMD<<(ADDR_LEN)) | addr;
	
	spi_port->ctrl0 = (CMD_LEN + ADDR_LEN) + 8*byte_cnt;            // TX: 32bits & RX: 8*byte_cnt

	//start transaction
	spi_port->spi_go = 0x1;     

	//wait transaction done
	spi_wait_ready();
 
	rd_data = spi_port->Rx0;

	buf[0] = (rd_data & 0xff000000) >> 24;	
	buf[1] = (rd_data & 0xff0000) >> 16;
	buf[2] = (rd_data & 0xff00) >> 8;
	buf[3] = (rd_data & 0xff);

	
}

int spi_write_bytes(unsigned int addr, unsigned int byte_cnt, unsigned int wr_data)
{
	unsigned int status;
 

	spi_port->Tx1   = (PAGE_PROG_CMD <<ADDR_LEN) | addr;
     spi_port->Tx0   = ((wr_data & 0xff)<<24) | ((wr_data & 0xff00)<<8) | ((wr_data & 0xff0000)>>8) | ((wr_data & 0xff000000)>>24);

	spi_port->ctrl0 = (CMD_LEN + ADDR_LEN) + 8*byte_cnt;            // TX: 32bits

	//start transaction
	spi_port->spi_go = 0x1;     

	//wait transaction done
	spi_wait_ready();

	//wait flash write finish & the flash write is auto disabled
	status = spi_write_status();
        spi_write_en();
 
	//TRACE(KERN_ERROR, "Flash Write Data=%x.\n", wr_data);
	
	return status;
}

static void spi_flash_addr(u32 addr, u8 *cmd)
{
	/* cmd[0] is actual command */
	cmd[1] = addr >> 16;
	cmd[2] = addr >> 8;
	cmd[3] = addr >> 0;
}

static int spi_flash_read_write(struct spi_slave *spi,
				const u8 *cmd, size_t cmd_len,
				const u8 *data_out, u8 *data_in,
				size_t data_len)
{
	unsigned int rd_data; 
	unsigned int wr_data;

	printf("cmd_len = %d data_len = %d\n",cmd_len,data_len);
	if((cmd_len + data_len) > 4)
	{
		printf("cmd_len = %d data_len = %d not implemented!\n",cmd_len,data_len);
		return -1;
	}

	if(cmd_len == 4)
		wr_data = (cmd[0]<<24) | (cmd[1] << 16) | (cmd[2] << 8) | cmd[3];
	else if(cmd_len == 3)
		wr_data = (cmd[0]<<16) | (cmd[1] << 8) | cmd[2];
	else if(cmd_len == 2)
		wr_data = (cmd[0]<<8) | cmd[1];
	else if(cmd_len == 1)
		wr_data = cmd[0];
	
	spi_port->Tx0 = wr_data << (data_len*8); 
	spi_port->ctrl0 = (cmd_len+data_len)*8;		     //TX: 8 & RX: 40
	spi_port->spi_go = 0x1;     
	spi_wait_ready();

	rd_data = spi_port->Rx0;

	if(data_len == 1)
		data_in[0] = rd_data&0xff;
	else if(data_len == 2)
	{
		data_in[0] = (rd_data & 0xff00) >> 8;
		data_in[1] = (rd_data & 0xff);
	}else if(data_len == 3)
	{
		data_in[0] = (rd_data & 0xff0000) >> 16;
		data_in[1] = (rd_data & 0xff00) >> 8;
		data_in[2] = (rd_data & 0xff);
	}else
	{
		//printf("data_len = %d should not go here!\n",data_len);
	}

	return 0;
}

int spi_flash_cmd(struct spi_slave *spi, u8 cmd, void *response, size_t len)
{
	return spi_flash_cmd_read(spi, &cmd, 1, response, len);
}

int spi_flash_cmd_read(struct spi_slave *spi, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len)
{
	return spi_flash_read_write(spi, cmd, cmd_len, NULL, data, data_len);
}

int spi_flash_cmd_write(struct spi_slave *spi, const u8 *cmd, size_t cmd_len,
		const void *data, size_t data_len)
{
	return spi_flash_read_write(spi, cmd, cmd_len, data, NULL, data_len);
}

int spi_flash_cmd_write_multi(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf)
{

	unsigned int  temp; 
	unsigned int  cycle_cnt;
	unsigned int  tail_bcnt;
	unsigned int  status; 
	int i;
	unsigned int *addr = (unsigned int *)buf;

	cycle_cnt  = len>>2;
	tail_bcnt  = len%4;

	if (tail_bcnt != 0)
		cycle_cnt = cycle_cnt + 1;

	//must erase before program page
	spi_write_en();

	for (i=0; i<cycle_cnt; i++)
	{ 
		temp = *(addr + i);

		//TRACE(KERN_DEBUG, "Flash Burn at word %d : %x\n", i, temp);

		//write 4 byte each time
		status = spi_write_bytes(offset+4*i, 4, temp);

		//Read back test
		//spi_read_bytes(4*i, 4, buf);

		//TRACE(KERN_DEBUG, "Flash Read Back %x\n", (*((unsigned int *) buf)));

		if(status) break;
	}
	debug("SF: program %s %zu bytes @ %#x\n",
			status ? "failure" : "success", len, offset);
	return status;
}

int spi_flash_read_common(struct spi_flash *flash, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len)
{
	struct spi_slave *spi = flash->spi;
	int ret;

	spi_claim_bus(spi);
	ret = spi_flash_cmd_read(spi, cmd, cmd_len, data, data_len);
	spi_release_bus(spi);

	return ret;
}

int spi_flash_cmd_read_fast(struct spi_flash *flash, u32 offset,
		size_t len, void *data)
{
	unsigned char temp[4]; //128 bit buffer
	unsigned int  cycle_cnt;
	unsigned int  tail_bcnt;
	unsigned int  flash_addr;
	int i,j;

	unsigned char *buf = (unsigned char *)data;

	flash_addr= offset & ((1<<ADDR_LEN)-1);

	cycle_cnt  = len>>2;
	tail_bcnt  = len%4;

	if (tail_bcnt != 0)
		cycle_cnt = cycle_cnt + 1;

	for (i=0; i<cycle_cnt; i++)
	{ 
		//read 4 byte each time
		spi_read_bytes(flash_addr, 4, temp);
		//printf("RF %x\n", *((unsigned int * )temp));

		for (j=0; j<4; j++)
		{
			*buf = temp[j];
			buf++;
		}  

		flash_addr +=4;
	}

	return 0;
}

int spi_flash_cmd_poll_bit(struct spi_flash *flash, unsigned long timeout,
		u8 cmd, u8 poll_bit)
{
	int ret;
	u8 status;
	
	do {

		ret = spi_flash_cmd_write(flash->spi, &cmd, 1, &status, 1);
		if (ret)
			return -1;

		if ((status & poll_bit) == 0)
			break;

		timeout--;
	} while (timeout > 0);


	if ((status & poll_bit) == 0)
		return 0;

	/* Timed out */
	debug("SF: time out!\n");
	return -1;
}

int spi_flash_cmd_wait_ready(struct spi_flash *flash, unsigned long timeout)
{
	return spi_flash_cmd_poll_bit(flash, timeout,
			CMD_READ_STATUS, STATUS_WIP);
}

int spi_flash_cmd_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	u32 start, end, erase_size;
	int ret;
	u8 cmd[4];

	erase_size = flash->sector_size;
	if (offset % erase_size || len % erase_size) {
		debug("SF: Erase offset/length not multiple of erase size\n");
		return -1;
	}

	if (erase_size == 4096)
		cmd[0] = CMD_ERASE_4K;
	else
		cmd[0] = CMD_ERASE_64K;
	start = offset;
	end = start + len;

	while (offset < end) {
		spi_flash_addr(offset, cmd);
		offset += erase_size;

		debug("SF: erase %2x %2x %2x %2x (%x)\n", cmd[0], cmd[1],
				cmd[2], cmd[3], offset);

		spi_write_en();

		ret = spi_flash_cmd_write(flash->spi, cmd, sizeof(cmd), NULL, 0);
		if (ret)
			goto out;

		ret = spi_write_status();
		debug("spi_write_status return %d\n",ret);
	}

	debug("SF: Successfully erased %zu bytes @ %#x\n", len, start);

out:
	return ret;
}

int spi_flash_cmd_write_status(struct spi_flash *flash, u8 sr)
{
	u8 cmd;
	int ret;

	ret = spi_flash_cmd_write_enable(flash);
	if (ret < 0) {
		debug("SF: enabling write failed\n");
		return ret;
	}

	cmd = CMD_WRITE_STATUS;
	ret = spi_flash_cmd_write(flash->spi, &cmd, 1, &sr, 1);
	if (ret) {
		debug("SF: fail to write status register\n");
		return ret;
	}

	ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
	if (ret < 0) {
		debug("SF: write status register timed out\n");
		return ret;
	}

	return 0;
}

/*
 * The following table holds all device probe functions
 *
 * shift:  number of continuation bytes before the ID
 * idcode: the expected IDCODE or 0xff for non JEDEC devices
 * probe:  the function to call
 *
 * Non JEDEC devices should be ordered in the table such that
 * the probe functions with best detection algorithms come first.
 *
 * Several matching entries are permitted, they will be tried
 * in sequence until a probe function returns non NULL.
 *
 * IDCODE_CONT_LEN may be redefined if a device needs to declare a
 * larger "shift" value.  IDCODE_PART_LEN generally shouldn't be
 * changed.  This is the max number of bytes probe functions may
 * examine when looking up part-specific identification info.
 *
 * Probe functions will be given the idcode buffer starting at their
 * manu id byte (the "idcode" in the table below).  In other words,
 * all of the continuation bytes will be skipped (the "shift" below).
 */
#define IDCODE_CONT_LEN 0
#define IDCODE_PART_LEN 5
static const struct {
	const u8 shift;
	const u8 idcode;
	struct spi_flash *(*probe) (struct spi_slave *spi, u8 *idcode);
} flashes[] = {
	/* Keep it sorted by define name */
#ifdef CONFIG_SPI_FLASH_ATMEL
	{ 0, 0x1f, spi_flash_probe_atmel, },
#endif
#ifdef CONFIG_SPI_FLASH_EON
	{ 0, 0x1c, spi_flash_probe_eon, },
#endif
#ifdef CONFIG_SPI_FLASH_MACRONIX
	{ 0, 0xc2, spi_flash_probe_macronix, },
#endif
#ifdef CONFIG_SPI_FLASH_SPANSION
	{ 0, 0x01, spi_flash_probe_spansion, },
#endif
#ifdef CONFIG_SPI_FLASH_SST
	{ 0, 0xbf, spi_flash_probe_sst, },
#endif
#ifdef CONFIG_SPI_FLASH_STMICRO
	{ 0, 0x20, spi_flash_probe_stmicro, },
#endif
#ifdef CONFIG_SPI_FLASH_WINBOND
	{ 0, 0xef, spi_flash_probe_winbond, },
#endif
#ifdef CONFIG_SPI_FRAM_RAMTRON
	{ 6, 0xc2, spi_fram_probe_ramtron, },
# undef IDCODE_CONT_LEN
# define IDCODE_CONT_LEN 6
#endif
	/* Keep it sorted by best detection */
#ifdef CONFIG_SPI_FLASH_STMICRO
	{ 0, 0xff, spi_flash_probe_stmicro, },
#endif
#ifdef CONFIG_SPI_FRAM_RAMTRON_NON_JEDEC
	{ 0, 0xff, spi_fram_probe_ramtron, },
#endif
};
#define IDCODE_LEN (IDCODE_CONT_LEN + IDCODE_PART_LEN)

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int spi_mode)
{
	struct spi_slave *spi = NULL;
	struct spi_flash *flash = NULL;
	int ret, i, shift;
	u8 idcode[IDCODE_LEN], *idp;

	newchip_spi_init();
	/* Read the ID codes */
	ret = spi_read_id(idcode);
	if (ret)
		goto err_read_id;

#ifdef DEBUG
	printf("SF: Got idcodes\n");
	print_buffer(0, idcode, 1, sizeof(idcode), 0);
#endif

	/* count the number of continuation bytes */
	for (shift = 0, idp = idcode;
			shift < IDCODE_CONT_LEN && *idp == 0x7f;
			++shift, ++idp)
		continue;

	/* search the table for matches in shift and id */
	for (i = 0; i < ARRAY_SIZE(flashes); ++i)
		if (flashes[i].shift == shift && flashes[i].idcode == *idp) {
			/* we have a match, call probe */
			flash = flashes[i].probe(spi, idp);
			if (flash)
				break;
		}

	if (!flash) {
		printf("SF: Unsupported manufacturer %02x\n", *idp);
		goto err_manufacturer_probe;
	}

	printf("SF: Detected %s with page size ", flash->name);
	print_size(flash->sector_size, ", total ");
	print_size(flash->size, "\n");

	return flash;

err_manufacturer_probe:
err_read_id:
	return NULL;
}

void spi_flash_free(struct spi_flash *flash)
{
	//	spi_free_slave(flash->spi);
	free(flash);
}
