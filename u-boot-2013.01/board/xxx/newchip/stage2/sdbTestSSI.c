#include "stdlib.h"

#include "sys.h"
#include "hal.h"
#include "sdbi.h"

#define REG_SSI_BaseAddr 0x21000000


#if defined(HAL_INC_SSI)

//============================
//== SSI read status check ===
//============================
void SSI_readstatus()
{
	unsigned int busy;
	unsigned int tmp_status;
	
	tmp_status = HAL_GETREG(REG_SSI_BaseAddr + 0x00);	
	busy = tmp_status && 0x00000001;
	while(busy)// 0 => jump out, 1 => go into {}
	{
	tmp_status = HAL_GETREG(REG_SSI_BaseAddr + 0x00);	
	busy = tmp_status && 0x00000001;	
	}
	
}

//============================
//== SSI Write status check ==
//============================
void SSI_write_en()
{
	HAL_SETREG32(REG_SSI_BaseAddr + 0x20, 0x06 );//WREN, Write Enable
	HAL_SETREG32(REG_SSI_BaseAddr + 0x04, 0x08 );
	HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
	SSI_readstatus();
}

void SSI_writestatus()
{
	unsigned int busy;
	unsigned int tmp_status;
	unsigned int SRWD, WEL, WIP;
	
	//ReadBack the status from Serial Flash
	HAL_SETREG32(REG_SSI_BaseAddr + 0x20, 0x0500 );//RDSR, Read Status Register
	HAL_SETREG32(REG_SSI_BaseAddr + 0x04, 0x10 );
	HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
	SSI_readstatus();
	
	tmp_status = HAL_GETREG(REG_SSI_BaseAddr + 0x10);
	
	SRWD = (tmp_status >> 7) & 0x00000001;
	WEL  = (tmp_status >> 1) & 0x00000001;
		
//	if( SRWD || !WEL)
	if( SRWD )
		{
			sdbvPrintf_g("Serial Flash is writing protected\r\n");
		}

	//Check WIP (Write in pregress bit)
	WIP = tmp_status & 0x00000001;
	
	//sdbvPrintf_g("status = %x\r\n",tmp_status);
	//sdbvPrintf_g("busy = %x\r\n",busy);
	
	while(WIP)// 0 => jump out, 1 => go into {}
	{
	//send the RDSR command again
	HAL_SETREG32(REG_SSI_BaseAddr + 0x20, 0x0500 );//RDSR, Read Status Register
	HAL_SETREG32(REG_SSI_BaseAddr + 0x04, 0x10 );
	HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
	//SSI_readstatus();
	
  //sdbvPrintf_g("status = %x\r\n",tmp_status);
	//sdbvPrintf_g("busy = %x\r\n",busy);

	tmp_status = HAL_GETREG(REG_SSI_BaseAddr + 0x10);	
	WIP = tmp_status & 0x00000001;	
	}
  //sdbvPrintf_g("Writing is finished\r\n");
}

//============================
//======= SSI initial ========
//============================
void sdbTest_SSI_ini()
{
HAL_SETREG32(REG_SSI_BaseAddr + 0x08, 0x6600 );	//Main control
HAL_SETREG32(REG_SSI_BaseAddr + 0x0C, 0x10000 );//[15:0] clk_div, 0 mins 2 times of sys_clk
HAL_SETREG32(REG_SSI_BaseAddr + 0x04, 0x08 );	  //char_len

HAL_SETREG32(REG_SSI_BaseAddr + 0x20, 0x06 );	  //WREN
HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );		//start
SSI_readstatus();	

HAL_SETREG32(REG_SSI_BaseAddr + 0x20, 0x98 );	  //GBULK
HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );		//start
SSI_readstatus();
SSI_writestatus();	

HAL_SETREG32(REG_SSI_BaseAddr + 0x20, 0x06 );	  //WREN
HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );		//start
SSI_readstatus();	

//Initial TV encoder
HAL_SETREG32(0x3023004C, 0x3E000000);
HAL_SETREG32(0x30230050, 0x3E0005A0);
HAL_SETREG32(0x30230054, 0x000005A0);
HAL_SETREG32(0x30230048, 0x00000011);
HAL_SETREG32(0x30230070, 0x00000000);

}

//============================
//====== SSI read back =======
//============================
void SSI_read_back (unsigned int tmp_data0, unsigned int tmp_data1, unsigned int tmp_data2, unsigned int address)
{
      unsigned int read_out2, read_out1, read_out0;
      unsigned int input_data2, input_data1, input_data0;
		
		  input_data2 = ( ((tmp_data2&0xFF)<<24) + ((tmp_data2&0xFF00)<<8) + ((tmp_data2&0xFF0000)>>8) + ((tmp_data2&0xFF000000)>>24) );  
		  input_data1 = ( ((tmp_data1&0xFF)<<24) + ((tmp_data1&0xFF00)<<8) + ((tmp_data1&0xFF0000)>>8) + ((tmp_data1&0xFF000000)>>24) );  
		  input_data0 = ( ((tmp_data0&0xFF)<<24) + ((tmp_data0&0xFF00)<<8) + ((tmp_data0&0xFF0000)>>8) + ((tmp_data0&0xFF000000)>>24) );  
		
			HAL_SETREG32(REG_SSI_BaseAddr + 0x2C, ((0x03 << 24) + address));
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04,0x00);//128bits
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start		
			SSI_readstatus();	
			read_out2 = HAL_GETREG(REG_SSI_BaseAddr + 0x10);
			read_out1 = HAL_GETREG(REG_SSI_BaseAddr + 0x14);
			read_out0 = HAL_GETREG(REG_SSI_BaseAddr + 0x18);	
			
			//Re-Write	
			if((read_out0!=input_data0)|(read_out1!=input_data1)|(read_out2!=input_data2))
			{
	    HAL_SETREG32(REG_SSI_BaseAddr + 0x20,input_data2 );
			HAL_SETREG32(REG_SSI_BaseAddr + 0x24,input_data1 );
			HAL_SETREG32(REG_SSI_BaseAddr + 0x28,input_data0 );
										
			HAL_SETREG32(REG_SSI_BaseAddr + 0x2C, ((0x02 << 24) + address));
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04,0x00);//128bits
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
			SSI_readstatus();			
			SSI_writestatus();
			SSI_write_en();//Auto write disable after data write
			
			HAL_SETREG32(REG_SSI_BaseAddr + 0x2C, ((0x03 << 24) + address));
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04,0x00);//128bits
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start		
			SSI_readstatus();	
			read_out2 = HAL_GETREG(REG_SSI_BaseAddr + 0x10);
			read_out1 = HAL_GETREG(REG_SSI_BaseAddr + 0x14);
			read_out0 = HAL_GETREG(REG_SSI_BaseAddr + 0x18);	
				if((read_out0!=input_data0)|(read_out1!=input_data1)|(read_out2!=input_data2))			
					{
						sdbvPrintf_g("Error happens @ %x\r\n", address);
					}
				else
					{
						sdbvPrintf_g("Rewrite ok @ %x\r\n", address);
					}
			}		
}

//============================
//========= SSI READ =========
//============================
void sdbTest_SSI_read(unsigned int ssi_m_addr,unsigned int ssi_s_addr,unsigned int ssi_size)
{
	unsigned int tmp_data0 , tmp_data1, tmp_data2;
	unsigned int nIdx, cycle, cycle_rem;
	
	sdbTest_SSI_ini();

	cycle 		= ssi_size / 12;
	cycle_rem = (ssi_size - cycle * 12);
	if(cycle_rem == 0)
		{
		cycle = cycle;
		}
  else
  	{
  	cycle = cycle + 1;
  	}
	
	sdbvPrintf_g("SSI reading cycles = %d (Decimal)\r\n",cycle);

	for (nIdx = 0; nIdx < cycle ; nIdx += 1)
		{
			HAL_SETREG32(REG_SSI_BaseAddr + 0x2C, ((0x03 << 24) + nIdx*12 + ssi_s_addr));
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04, 0x0 );//128bits, 16 bytes (12bytes for data)			
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
			SSI_readstatus();
			tmp_data2 = HAL_GETREG(REG_SSI_BaseAddr + 0x10);
			tmp_data1 = HAL_GETREG(REG_SSI_BaseAddr + 0x14);
			tmp_data0 = HAL_GETREG(REG_SSI_BaseAddr + 0x18);
			
			
			tmp_data2 =	((tmp_data2&0xFF)<<24)+((tmp_data2&0xFF00)<<8)+((tmp_data2&0xFF0000)>>8)+((tmp_data2&0xFF000000)>>24) ;
			tmp_data1 =	((tmp_data1&0xFF)<<24)+((tmp_data1&0xFF00)<<8)+((tmp_data1&0xFF0000)>>8)+((tmp_data1&0xFF000000)>>24) ;
			tmp_data0 =	((tmp_data0&0xFF)<<24)+((tmp_data0&0xFF00)<<8)+((tmp_data0&0xFF0000)>>8)+((tmp_data0&0xFF000000)>>24) ;
					
			
			HAL_SETREG32(ssi_m_addr + (nIdx*3 + 0)*4,tmp_data0);
			HAL_SETREG32(ssi_m_addr + (nIdx*3 + 1)*4,tmp_data1);
			HAL_SETREG32(ssi_m_addr + (nIdx*3 + 2)*4,tmp_data2);	
		}

	tmp_data2 = HAL_GETREG(REG_SSI_BaseAddr + 0x10);
	tmp_data1 = HAL_GETREG(REG_SSI_BaseAddr + 0x14);
	tmp_data0 = HAL_GETREG(REG_SSI_BaseAddr + 0x18);

	sdbvPrintf_g("%x\r\n",tmp_data0);
	sdbvPrintf_g("%x\r\n",tmp_data1);
	sdbvPrintf_g("%x\r\n",tmp_data2);

}

//============================
//========= SSI Write ========
//============================
void sdbTest_SSI_write(unsigned int ssi_m_addr,unsigned int ssi_s_addr,unsigned int ssi_size)
{
	unsigned int tmp_data0 , tmp_data1, tmp_data2;
	unsigned int new_data0 , new_data1, new_data2;
	unsigned int nIdx, nIdy, cycle, cycle_rem;	
	unsigned int page_256, ssi_size_rem;
	unsigned int address;
	
	sdbTest_SSI_ini();
	HAL_SETREG32(REG_SSI_BaseAddr + 0x04, 0x0 );//128bits, 16 bytes (12bytes for data)
	
	page_256 = ssi_size /256;
	
	
	sdbvPrintf_g("SSI Write cycles = %d (Decimal)\r\n",page_256);

	SSI_write_en();

for (nIdy = 0; nIdy < page_256 ; nIdy += 1)
	{
			for (nIdx = 0; nIdx < 21 ; nIdx += 1)
		{
			tmp_data0 = HAL_GETREG(ssi_m_addr + (nIdy*256) + (nIdx*3 + 0)*4);
			tmp_data1 = HAL_GETREG(ssi_m_addr + (nIdy*256) + (nIdx*3 + 1)*4);
			tmp_data2 = HAL_GETREG(ssi_m_addr + (nIdy*256) + (nIdx*3 + 2)*4);
			
			HAL_SETREG32(REG_SSI_BaseAddr + 0x20,((tmp_data2&0xFF)<<24)+((tmp_data2&0xFF00)<<8)+((tmp_data2&0xFF0000)>>8)+((tmp_data2&0xFF000000)>>24) );
			HAL_SETREG32(REG_SSI_BaseAddr + 0x24,((tmp_data1&0xFF)<<24)+((tmp_data1&0xFF00)<<8)+((tmp_data1&0xFF0000)>>8)+((tmp_data1&0xFF000000)>>24) );
			HAL_SETREG32(REG_SSI_BaseAddr + 0x28,((tmp_data0&0xFF)<<24)+((tmp_data0&0xFF00)<<8)+((tmp_data0&0xFF0000)>>8)+((tmp_data0&0xFF000000)>>24) );
			
			HAL_SETREG32(REG_SSI_BaseAddr + 0x2C, ((0x02 << 24) + nIdy*256 + nIdx*12 + ssi_s_addr));
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04,0x00);//128bits
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
			SSI_readstatus();			
			SSI_writestatus();
			SSI_write_en();//Auto write disable after data write
			
			address = (nIdy*256 + nIdx*12 + ssi_s_addr);
			SSI_read_back(tmp_data0, tmp_data1, tmp_data2, address);
		}
		//for 253 254 255 256 (32bits, 4bytes)
			tmp_data0 = HAL_GETREG(ssi_m_addr + (nIdy*256) + (21*3 + 0)*4);
			HAL_SETREG32(REG_SSI_BaseAddr + 0x20,((tmp_data0&0xFF)<<24)+((tmp_data0&0xFF00)<<8)+((tmp_data0&0xFF0000)>>8)+((tmp_data0&0xFF000000)>>24) );  
			HAL_SETREG32(REG_SSI_BaseAddr + 0x24, ((0x02 << 24) + nIdy*256 + 21*12 + ssi_s_addr));
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04,0x40);//64bits
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
			SSI_readstatus();			
			SSI_writestatus();
			SSI_write_en();//Auto write disable after data write		
			
			new_data0 = tmp_data1;
			new_data1 = tmp_data2;
			new_data2 = tmp_data0;
			
			address = (nIdy*256 + 21*12 + ssi_s_addr - 8);
			SSI_read_back(new_data0, new_data1, new_data2, address);	
		
	}

// Remainder after /256
  ssi_size_rem = ssi_size - page_256*256;
   
	cycle 		= ssi_size_rem / 12;
	
	cycle_rem = (ssi_size_rem - cycle * 12);
	if(cycle_rem == 0)
		{
		cycle = cycle;
		}
  else
  	{
  	cycle = cycle + 1;
  	}

	for (nIdx = 0; nIdx < cycle ; nIdx += 1)
		{
			tmp_data0 = HAL_GETREG(ssi_m_addr + (nIdy*256) + (nIdx*3 + 0)*4);
			tmp_data1 = HAL_GETREG(ssi_m_addr + (nIdy*256) + (nIdx*3 + 1)*4);
			tmp_data2 = HAL_GETREG(ssi_m_addr + (nIdy*256) + (nIdx*3 + 2)*4);
			
			HAL_SETREG32(REG_SSI_BaseAddr + 0x20,((tmp_data2&0xFF)<<24)+((tmp_data2&0xFF00)<<8)+((tmp_data2&0xFF0000)>>8)+((tmp_data2&0xFF000000)>>24) );
			HAL_SETREG32(REG_SSI_BaseAddr + 0x24,((tmp_data1&0xFF)<<24)+((tmp_data1&0xFF00)<<8)+((tmp_data1&0xFF0000)>>8)+((tmp_data1&0xFF000000)>>24) );
			HAL_SETREG32(REG_SSI_BaseAddr + 0x28,((tmp_data0&0xFF)<<24)+((tmp_data0&0xFF00)<<8)+((tmp_data0&0xFF0000)>>8)+((tmp_data0&0xFF000000)>>24) );
			
			HAL_SETREG32(REG_SSI_BaseAddr + 0x2C, ((0x02 << 24) + (nIdy*256) + nIdx*12 + ssi_s_addr));
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04,0x00);//128bits
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
			SSI_readstatus();			
			SSI_writestatus();
			SSI_write_en();//Auto write disable after data write
			
			address = (nIdy*256 + nIdx*12 + ssi_s_addr);
			SSI_read_back(tmp_data0, tmp_data1, tmp_data2, address);
		}
	
}

//============================
//========= SSI CMD ==========
//============================
void sdbTest_SSI_cmd(unsigned int ssi_data, unsigned int ssi_length, unsigned int ssi_read)
{
	unsigned int tmp_data;
	
	sdbTest_SSI_ini();
	
	if(ssi_read)
		{
			HAL_SETREG32(REG_SSI_BaseAddr + 0x20, ssi_data);
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04, ssi_length);//128bits
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
			SSI_readstatus();
			tmp_data = HAL_GETREG(REG_SSI_BaseAddr + 0x10);
			sdbvPrintf_g("Data = %x\r\n",tmp_data);
			
			//SSI_writestatus();
		}
	else
		{
			SSI_write_en();
			HAL_SETREG32(REG_SSI_BaseAddr + 0x20, ssi_data);
			HAL_SETREG32(REG_SSI_BaseAddr + 0x04, ssi_length);//128bits
			HAL_SETREG32(REG_SSI_BaseAddr + 0x00, 0x1 );//start
			SSI_readstatus();
			SSI_writestatus();
		}
	
}




#if 1

void sdbTest_SSI(void)
{
	unsigned char nStsRegVal;
	unsigned char nData[4];
	unsigned int nIdx;
	unsigned char nRandData[512];

	halSSI_SPISetup(SDB_SYS_CLOCK);

	nStsRegVal = halAT250x0AReadStatus();
	sdbvPrintf_g("MX25L6445E status register value = 0x%02X.\r\n", nStsRegVal);
	sdbvPrintf_g("MX25L6445E is protected at level %d.\r\n", (nStsRegVal&0x1C) >> 2);
	if (nStsRegVal & 0x1C)
	{
		sdbvPrintf_g("Change MX25L6445E protection level to 0.\r\n");
		halAT250x0AWriteStatus(nStsRegVal & 0xC3);


		while (1)
		{
			/* wait for the status register change */

			nStsRegVal = halAT250x0AReadStatus();
			if (nStsRegVal & 0x1C)
				;
			else
				break;
		}
		sdbvPrintf_g("New MX25L6445E status register value = 0x%02X.\r\n", nStsRegVal);
	}
		nData[0]=0xa5;
		nData[1]=0xa5;
		nData[2]=0xa5;
		nData[3]=0xa5;

	sdbvPrintf_g("read data from the MX25L6445E serial flash\r\n");
	for (nIdx = 0; nIdx < 256/4; ++nIdx)
	{
		MX25L6445E_ReadData(nIdx*4  , nData[0]);
		MX25L6445E_ReadData(nIdx*4+1, nData[1]);
		MX25L6445E_ReadData(nIdx*4+2, nData[2]);
		MX25L6445E_ReadData(nIdx*4+3, nData[3]);
		
//Turn off massage		
		sdbvPrintf_g("\tMX25L6445E [%3d] is 0x%02X  0x%02X  0x%02X  0x%02X\r\n", nIdx, nData[0],nData[1],nData[2],nData[3]);
		nData[0]=0xa5;
		nData[1]=0xa5;
		nData[2]=0xa5;
		nData[3]=0xa5;
	}


	/* generate 512 random datum */
	for (nIdx = 0; nIdx < 256; ++nIdx)
		nRandData[nIdx] = (unsigned char) (rand() & 0xFF);


	/* write the 512 random datum to the chip */
	sdbvPrintf_g("\r\n\nwrite a set of random data to the AT29F040 chip\r\n");
	for (nIdx = 0; nIdx < 256; )
	{
		nStsRegVal = halAT250x0AReadStatus();
		if ((nStsRegVal & 0x0F) == 0x02)
		{
			halAT250x0AWriteData(nIdx, nRandData[nIdx]);
			++nIdx;
		}
	}

	sdbvPrintf_g("read data from the MX25L6445E serial flash\r\n");
	for (nIdx = 0; nIdx < 256/4; ++nIdx)
	{
		halAT250x0AReadData(nIdx*4  , nData[0]);
		halAT250x0AReadData(nIdx*4+1, nData[1]);
		halAT250x0AReadData(nIdx*4+2, nData[2]);
		halAT250x0AReadData(nIdx*4+3, nData[3]);
		
//Turn off massage		
		sdbvPrintf_g("\tMX25L6445E [%3d] is 0x%02X  0x%02X  0x%02X  0x%02X\r\n", nIdx, nData[0],nData[1],nData[2],nData[3]);
		nData[0]=0x00;
		nData[1]=0x00;
		nData[2]=0x00;
		nData[3]=0x00;
	}
	
	/* read data from chip and verify */
//	sdbvPrintf_g("\r\n\nverify the result\r\n");
//	for (nIdx = 0; nIdx < 256; ++nIdx)
//	{
//		halAT250x0AReadData(nIdx, nData[0]);
//Turn off massage		
//		sdbvPrintf_g("\twrite AT29040A[%3d] ... %s\r\n", nIdx,
//			(nData[0] == nRandData[nIdx] ? "ok" : "fail"));
//	}

//Turn Off Protect
	/* protect from writing */
//	halAT250x0AWriteStatus(0x0E);

}


#else

#define SSI_DIV			40		/* with 60MHz */

//#define SSI_DIV			68			/* 25MHz or 30MHz on all three test chips */

//#define SSI_DIV			48			/* with 15MHz */

#define AT040A_WREN		0x06	/* 0000_0110 */
#define AT040A_WRDI		0x08	/* 0000_1000 */
#define AT040A_RDSR		0x05	/* 0000_0101 */
#define AT040A_WRSR		0x01	/* 0000_0001 */
#define AT040A_READ0	0x03	/* 0000_0011 */
#define AT040A_READ1	0x0B	/* 0000_1011 */
#define AT040A_WRITE0	0x02	/* 0000_0010 */
#define AT040A_WRITE1	0x0A	/* 0000_1010 */

void sdbTest_SSI(void)
{
	int i;
	unsigned short wbuf[512], rbuf[512];


	halSSI_SPISetup(SSI_DIV, 8);

	/* write enable */
	wbuf[0] = AT040A_WREN;
	halSSI_SPIWrite(wbuf, 1);

	/* block write protection : level 0 : none */
	wbuf[0] = AT040A_WRSR;
	wbuf[1] = 0x2;
	halSSI_SPIWrite(wbuf, 2);


	sdbvPrintf_g("\r\nread contents on the IC");
	for (i = 0; i < 512; )
	{
		/* write enable */
		wbuf[0] = AT040A_WREN;
		halSSI_SPIWrite(wbuf, 1);

		/* read status */
		wbuf[0] = AT040A_RDSR;
		rbuf[0] = 6;
		rbuf[1] = 5;
		halSSI_SPIWriteRead(wbuf, rbuf, 1, 1);
    
		
		/* read one byte */
		if ((rbuf[0] & 0x3) == 0x2)
		{
			if (i < 256)
			{
				wbuf[0] = AT040A_READ0;
				wbuf[1] = i;
			}
			else
			{
				wbuf[0] = AT040A_READ1;
				wbuf[1] = i-256;
			}

			halSSI_SPIWriteRead(wbuf, rbuf, 2, 1);

			sdbvPrintf_g("\r\ni = %03x, rbuf[0] = 0x%x", i, rbuf[0]);
			
			++i;
		}

		/* write disable */
		//wbuf[0] = AT040A_WREN;
		//wbuf[1] = AT040A_WRDI;
		//halSSI_SPIWrite(wbuf, 2);
	}
	sdbvPrintf_g("\r\n");



	sdbvPrintf_g("\r\nwrite values to each address");
	for (i = 0; i < 512; )
	{
		/* write enable */
		wbuf[0] = AT040A_WREN;
		halSSI_SPIWrite(wbuf, 1);

		/* read status */
		wbuf[0] = AT040A_RDSR;
		rbuf[0] = 6;
		halSSI_SPIWriteRead(wbuf, rbuf, 1, 1);
    
		
		/* write one byte */
		if ((rbuf[0] & 0x3) == 0x2)
		{
			if (i < 256)
			{
				wbuf[0] = AT040A_WRITE0;
				wbuf[1] = i;
			}
			else
			{
				wbuf[0] = AT040A_WRITE1;
				wbuf[1] = i-256;
			}
			wbuf[2] = (i*i)%256;

			halSSI_SPIWrite(wbuf, 3);
			
			++i;
		}

		/* write disable */
		//wbuf[0] = AT040A_WREN;
		//wbuf[1] = AT040A_WRDI;
		//halSSI_SPIWrite(wbuf, 2);
	}
	sdbvPrintf_g("\r\n");



	sdbvPrintf_g("\r\nread values from the IC");
	for (i = 0; i < 512; )
	{
		/* write enable */
		wbuf[0] = AT040A_WREN;
		halSSI_SPIWrite(wbuf, 1);

		/* read status */
		wbuf[0] = AT040A_RDSR;
		rbuf[0] = 6;
		halSSI_SPIWriteRead(wbuf, rbuf, 1, 1);
    
		
		/* read one byte */
		if ((rbuf[0] & 0x3) == 0x2)
		{
			if (i < 256)
			{
				wbuf[0] = AT040A_READ0;
				wbuf[1] = i;
			}
			else
			{
				wbuf[0] = AT040A_READ1;
				wbuf[1] = i-256;
			}

			halSSI_SPIWriteRead(wbuf, rbuf, 2, 1);

			if (rbuf[0] != ((i*i) %256))
			{
				sdbvPrintf_g("\r\ni = %03x, rbuf[0] = 0x%x, (i^2)%%256 = 0x%x",
					i, rbuf[0], i*i%256);
			}
			
			++i;
		}

		/* write disable */
		//wbuf[0] = AT040A_WREN;
		//wbuf[1] = AT040A_WRDI;
		//halSSI_SPIWrite(wbuf, 2);
	}

	/* write enable */
	//wbuf[0] = AT040A_WREN;
	//halSSI_SPIWrite(wbuf, 1);

	/* block write protection : level 0 : none */
	//wbuf[0] = AT040A_WRSR;
	//wbuf[1] = 0x0;
	//halSSI_SPIWrite(wbuf, 2);
}
#endif



#endif

