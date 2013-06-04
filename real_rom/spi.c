/*  ============================================================================
 */

#include "config.h"
#include "real_rom.h"
#include "spi.h"


#define spi_port ((UR0431A_SPI_t)SPI_0_BASE)

static int spi_check_device_id();
static void spi_read_block(unsigned int addr, unsigned int length, unsigned char * buf);

int spi_boot(void)
{
	int status = 0;
        int sec_cnt = 0;
        xl_header head;


	spi_init();

	status = spi_check_device_id();
		
	if (status)
	{
	  return status;
        }

	//read image header
	spi_read_block(0, XL_HEAD_SIZE,(unsigned char *)&head);	

	//check image header
        status = check_header(&head);
        if(status)
	{
		return status;
	}


	TRACE(KERN_DEBUG,"Start download codes @%x\n", head.entry);

	spi_read_block(head.offset, head.size, (unsigned char *)head.entry);

	if(checksum32((void*)head.entry, head.size) != head.loader_cksum)
        {
        	TRACE(KERN_ERROR, "checksum fail\n");
                return 1;
        }

	TRACE(KERN_DEBUG,"Ready to go use codes @%x\n", head.entry);

	enter_entry((void*)head.entry);
}

int spi_init(void)
{
        int clk_div_rate;
	//set work mode
        spi_port->ctrl1 = ASS_EN | MOSI_NEG_EN;

	//set spi clock
	clk_div_rate    = ((CFG_APB_CLOCK/CFG_SPI_CLOCK)>>1)-1;
   
        //use cs0 select slave 
        spi_port->ctrl2 = clk_div_rate | CS0_EN;

	return 0;	
}

unsigned int endian_swap(unsigned int bword)
{
         unsigned int lword;
         lword =  ((bword & 0xff000000) >> 24)
                 |((bword & 0x00ff0000) >> 8)
                 |((bword & 0x0000ff00) << 8)
                 |((bword & 0x000000ff) << 24);

	 return lword;
}

int spi_check_device_id()
{
        unsigned int rd_data;
	unsigned int cmd;
        //Try ID read and check with EXP_SLAVE_ID
        spi_port->Tx1 = (READ_ID_CMD<<(ADDR_LEN+ID_LEN-32)); 

	/////////////////////////
	// CTRL0 register
	/////////////////////////
	// Bit[6:0] (char_len) : Char_len = 7'b0   => 128bits
	//                       Char_len = 7'd127 => 127bits
	spi_port->ctrl0 = (CMD_LEN + ADDR_LEN + ID_LEN);            // TX: 32bits & RX: 16bits

	//start transaction
	spi_port->spi_go = 0x1;     

	// Status check
	do{
	     rd_data = spi_port->spi_go;
	}while(rd_data&0x1); 

 
	rd_data = spi_port->Rx0;
	rd_data = rd_data & ((0x1<<ID_LEN)-1);

	if (rd_data == EXP_SLAVE_ID)
	{
	     return 0;
        }
        else
	{  
	     TRACE(KERN_ERROR, "SPI check ID fail.\n");
	     return 1;
	}
}

void spi_read_bytes(unsigned int addr, unsigned int byte_cnt, unsigned char *buf)
{
        unsigned int rd_data;
	unsigned int cmd;
    
	spi_port->Tx1   = (READ_BYTES_CMD<<(ADDR_LEN)) | addr;
	
	spi_port->ctrl0 = (CMD_LEN + ADDR_LEN) + 8*byte_cnt;            // TX: 32bits & RX: 8*byte_cnt

	//start transaction
	spi_port->spi_go = 0x1;     

	// Status check
	do{
	     rd_data = spi_port->spi_go;
	}while(rd_data&0x1); 

 
	rd_data = spi_port->Rx0;

	buf[3] = (rd_data & 0xff000000) >> 24;	
	buf[2] = (rd_data & 0xff0000) >> 16;
	buf[1] = (rd_data & 0xff00) >> 8;
	buf[0] = (rd_data & 0xff);

	
}

void spi_read_block(unsigned int addr, unsigned int length, unsigned char * buf)
{
      unsigned char temp[4]; //128 bit buffer
      unsigned int  cycle_cnt;
      unsigned int  tail_bcnt;
      unsigned int  flash_addr;

      int i,j;

      flash_addr= addr & ((1<<ADDR_LEN)-1);

      cycle_cnt  = length>>2;
      tail_bcnt  = length%4;

      for (i=0; i<cycle_cnt; i++)
      { 
	  //read 4 byte each time
	  spi_read_bytes(flash_addr, 4, temp);
	  //printf("%c%c%c%c=>%x", temp[0], temp[1], temp[2], temp[3], *((unsigned int * )temp));
	
	  for (j=0; j<4; j++)
	  {
	      *buf = temp[j];
	       buf++;
	  }  
	
	  flash_addr +=4;
      }

      if (tail_bcnt)  //one more read for tail bytes
      {
	  spi_read_bytes(flash_addr, tail_bcnt, temp);
	  
	  switch(tail_bcnt) {
	  case 1:  *buf++ = temp[3];
	    break; 
	  case 2:  *buf++ = temp[2]; *buf++=temp[3];
	    break;
	  case 3:  *buf++ = temp[1]; *buf++=temp[2]; *buf++ = temp[3];
	    break;
	  }
      }
      
}

