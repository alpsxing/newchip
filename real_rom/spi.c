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
        spi_port->ctrl1 = WP_POS_EN | ASS_EN | MOSI_NEG_EN;

	//set spi clock
	clk_div_rate    = ((CFG_APB_CLOCK/CFG_SPI_CLOCK)>>1)-1;
   
        //use cs0 select slave 
        spi_port->ctrl2 = clk_div_rate | CS0_EN;

	return 0;	
}

void spi_wait_ready()
{
        unsigned int rd_data;
	
	// Status check
	do{
	     rd_data = spi_port->spi_go;
	}while(rd_data&0x1); 
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

	//wait for transaction done
	spi_wait_ready();

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

void spi_write_en()
{
       spi_port->Tx0   =  WRITE_EN_CMD;  
       spi_port->ctrl0 =  CMD_LEN;

	//start transaction
	spi_port->spi_go = 0x1;     

	//wait transaction done
	spi_wait_ready();

	//TRACE(KERN_ERROR, "Flash Write Enable.\n");
}


void spi_write_disable()
{
       spi_port->Tx0   =  WRITE_DIS_CMD;  
       spi_port->ctrl0 =  CMD_LEN;

	//start transaction
	spi_port->spi_go = 0x1;     

	//wait transaction done
	spi_wait_ready();

	//TRACE(KERN_ERROR, "Flash Write Enable.\n");
}

int spi_write_status()
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

void spi_erase()
{
       spi_port->Tx0   =  BULK_ERASE_CMD;  
       spi_port->ctrl0 =  CMD_LEN;

	//start transaction
	spi_port->spi_go = 0x1;     

	//wait transaction done
	spi_wait_ready();

	//wait flash write finish & the flash write is auto disabled
	spi_write_status();
        spi_write_en();

	TRACE(KERN_DEBUG, "Flash Bulk Erase.\n");
}

void spi_read_bytes(unsigned int addr, unsigned int byte_cnt, unsigned char *buf)
{
        unsigned int rd_data;
	unsigned int cmd;
    
	spi_port->Tx1   = (READ_BYTES_CMD<<(ADDR_LEN)) | addr;
	
	spi_port->ctrl0 = (CMD_LEN + ADDR_LEN) + 8*byte_cnt;            // TX: 32bits & RX: 8*byte_cnt

	//start transaction
	spi_port->spi_go = 0x1;     

	//wait transaction done
	spi_wait_ready();
 
	rd_data = spi_port->Rx0;

	buf[3] = (rd_data & 0xff000000) >> 24;	
	buf[2] = (rd_data & 0xff0000) >> 16;
	buf[1] = (rd_data & 0xff00) >> 8;
	buf[0] = (rd_data & 0xff);

	
}

int spi_write_bytes(unsigned int addr, unsigned int byte_cnt, unsigned int wr_data)
{
	unsigned int cmd;
	unsigned int status;
 

	spi_port->Tx1   = (PAGE_PROG_CMD <<ADDR_LEN) | addr;
	spi_port->Tx0   =  wr_data;

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
}

int spi_write_block(volatile unsigned int * addr, unsigned int length)
{
      unsigned int  temp; 
      unsigned int  cycle_cnt;
      unsigned int  tail_bcnt;
      unsigned int  source_addr;
      unsigned int  status;
      unsigned char buf[4];

      int i,j;

      cycle_cnt  = length>>2;
      tail_bcnt  = length%4;

      if (tail_bcnt != 0)
	cycle_cnt = cycle_cnt + 1;

      //must erase before program page
      spi_write_en();
      spi_erase();


      for (i=0; i<cycle_cnt; i++)
      { 
	  temp = *(addr + i);

	  //TRACE(KERN_DEBUG, "Flash Burn at word %d : %x\n", i, temp);

	  //write 4 byte each time
	  status = spi_write_bytes(4*i, 4, temp);

	  //Read back test
	  //spi_read_bytes(4*i, 4, buf);
	  
	  //TRACE(KERN_DEBUG, "Flash Read Back %x\n", (*((unsigned int *) buf)));

	  if(status) break;
      }

      
      
      return status;
}
