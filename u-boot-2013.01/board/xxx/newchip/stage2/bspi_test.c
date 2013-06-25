#include "config.h"
#include "common.h"
#include "arm_defines.h"

//--------------------------------------
// The main routine for the testcase.
//--------------------------------------
int bspi_test(void)
{
  int i;

  // TEST-SPECIFIC DECLARATIONS
  volatile bspi_regs *bspi_regs_ptr;            //declare a pointer to BSPI regs
  volatile unsigned int address, wr_data, rd_data;
  unsigned int expect_val;

  bspi_regs_ptr = (bspi_regs *)BSPI_BASE;       //init the BSPI regs pointer 

  TRACE(KERN_DEBUG,"CHKPT1: Entering BSPI Test\n");
  TRACE(KERN_DEBUG,"BSPI Init\n");

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
  bspi_regs_ptr->ctrl1 = 0x2400; 
                                            
                                            
                                            
                                            
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
  bspi_regs_ptr->ctrl2 = 0x10009;       // ASIC: 10Mhz SCLK (200Mhz/20)
                                            

  TRACE(KERN_DEBUG,"CHKPT2: Read Manufactuer and Device ID (READ_ID) Command Test\n");
  /////////////////////////
  // Tx3 register
  /////////////////////////
  // Tx3[31:24]: Command- 0x90 
  // Tx3[23:0]:  Address- 0x000000 
  bspi_regs_ptr->Tx1 = 0x00009000;       

  /////////////////////////
  // CTRL0 register
  /////////////////////////
  // Bit[6:0] (char_len) : Char_len = 7'b0   => 128bits
  //                       Char_len = 7'd127 => 127bits
  bspi_regs_ptr->ctrl0 = 48;            // TX: 32bits & RX: 16bits

  /////////////////////////
  // SPI_GO register
  /////////////////////////
  // Start 
  bspi_regs_ptr->spi_go = 0x1;            // TX: 32bits & RX: 16bits

  // Status check
  rd_data = bspi_regs_ptr->spi_go;

  while(rd_data&0x1) {
      rd_data = bspi_regs_ptr->spi_go;
  }

  TRACE(KERN_DEBUG,"READ_ID Command DONE\n");

  rd_data = bspi_regs_ptr->Rx0;
  rd_data = rd_data & 0xffff;
  TRACE(KERN_DEBUG,"READ_ID Command Data = %x\n", rd_data);

  expect_val = 0x0117;
  if (rd_data != expect_val)  {
    TRACE(KERN_DEBUG,"ERROR: read id = %x, expected = %x\n", rd_data, expect_val);
  }
  TRACE(KERN_DEBUG,"read id = %x, as expected\n", rd_data);


  TRACE(KERN_DEBUG,"Okay\n");
  TRACE(KERN_DEBUG,"--------------------\n");

}


