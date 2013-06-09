//*****************************************
// Data (Rx/Tx) test for the I2C peripheral
//
//  This test performs:
//  1) Checking of the SCL and SDA connections
//     using the I2C in master mode, with
//     a I2C EEPROM model
//  2) Interrupts (on Rx) will be checked (envetually)
//  3) Data rate will be varied.
//*****************************************
#ifndef I2C_TEST_HIGHSPEED__C
#define I2C_TEST_HIGHSPEED__C
#include "arm_defines.h"


#define WORD_ADDR 0x5A
#define WORD_DATA 0xA5

volatile unsigned wait_flag;

//--------------------------------------
// The main routine for the testcase.
//--------------------------------------
int main(void)
{
  volatile i2c_regs *i2c_regs_ptr;             // declare a pointer to the I2C regs
  unsigned int i, cmd_val, exp_val;
  volatile unsigned rd_val;

  i2c_regs_ptr = (i2c_regs *)I2C_BASE;         // init the pointer


  Display("CHKPT1: Entering I2C test\n");
  
  // Enable all interrupts, expecting only bit 10 
  InterruptEnable(0); // Enable all interrupts

  WaitLoop(10);

  // Initialize I2C for Master mode
  I2CInitMaster();
  Display("CHKPT2: I2C Init complete, TAR = 0x%x\n", i2c_regs_ptr->ic_tar);

  // Write One Byte to EEPROM
  sdv_note("Send Address & Data \n");
  CmdMaster(0);				// HIGH-word address
  CmdMaster(WORD_ADDR);		// Low-word address

  //sdv_note("Send Data\n");
  CmdMaster(WORD_DATA);

 // Wait for interrupt loop
  // In interrupt routine, set polling location to '1'
  rd_val = 0;
  wait_flag = 0x0;
   
  // unmask interrupt,  
  i2c_regs_ptr->ic_intr_mask = 0x200; // Bit9: Stop detect mask 
  
  while (rd_val == 0){
    rd_val = wait_flag;
  }
  
  WaitLoop(1000);

  Display("CHKPT5: I2C Write Interrupt processing done\n");

  // Read one byte
  sdv_note("Send Read Word Address & command \n");
  CmdMaster(0);				// HIGH- word address
  CmdMaster(WORD_ADDR);		// LOW- word address
  //sdv_note("Send read command");
  CmdMaster(0x100);

 // Wait for interrupt loop
  // In interrupt routine, set polling location to '1'
  rd_val = 0;
  wait_flag = 0;
   
  // unmask interrupt,  
  i2c_regs_ptr->ic_intr_mask = 0x200; // Bit9: Stop detect mask 
  
  while (rd_val == 0){
    rd_val = wait_flag;
  }

  //--Display("CHKPT8: I2C Read Interrupt processing done\n");

  // Check the data, 
  // Expecting same data that was sent + 0x10
  exp_val = WORD_DATA;
  rd_val = i2c_regs_ptr->ic_data_cmd;
  Display("****** rd_val= %x\n",rd_val);  //for debug
  if (rd_val != exp_val)
    _FAIL();
  Display("read byte = %x, as expected\n", exp_val);


  // Test Passed
  _PASS();
  _END();
  
}

//*********************************************
// Functions used in this test
//*********************************************

// Function for minimally initializing a I2C
int I2CInitMaster(void)
{
  volatile i2c_regs *i2c_regs_ptr;        // declare a pointer to the I2C regs
  volatile unsigned rd_val; 
  i2c_regs_ptr = (i2c_regs *)I2C_BASE;  // init the pointer 


  // disable i2c 
  i2c_regs_ptr->ic_enable = 0x0;   // Disable
  // mask all interrupts,  
  i2c_regs_ptr->ic_intr_mask = 0x0; 
  // i2c configuration 
  i2c_regs_ptr->ic_con = 0x65;   // Bit6: 0-slave enable, 1-slave disable
                                 // Bit5: 0-restart disable, 1-restart enable
                                 // Bit4: 0-7-bit addr, 1-10-bit addr (master)
                                 // Bit3: 0-7-bit addr, 1-10-bit addr (slave)
                                 // Bit[2:1]: 1-100kbs, 2-400kbs, 3-3.4Mbs (max speed)
                                 // Bit0: 0-master disable, 1-master enable

  // i2c target address 
  i2c_regs_ptr->ic_tar = 0x0050;   // Bit12: 0-7-bit addr, 1-10-bit addr
                                   // Bit11: Special
                                   // Bit10: GC or Start
                                   // Bit[9:0]: Target Address : 1010_000

  // setup and hold
  i2c_regs_ptr->ic_sda_hold = 0x3;   // 
  i2c_regs_ptr->ic_sda_setup = 0x3;   // 

  i2c_regs_ptr->ic_hs_scl_lcnt = 0x8;
  i2c_regs_ptr->ic_hs_scl_hcnt = 0x6;

  // enable i2c 
  i2c_regs_ptr->ic_enable = 1;   // Enable, when '0' holds fifos in erased state  

  return;
}

// Function for sending a Wr or Rd command
int CmdMaster(unsigned cmd_val)
{
  volatile i2c_regs *i2c_regs_ptr;        // declare a pointer to the I2C regs
  i2c_regs_ptr = (i2c_regs *)I2C_BASE;  // init the pointer 

  // Write or Read a byte 
  i2c_regs_ptr->ic_data_cmd = cmd_val; // Bit8: 0-write, 1-read (command direction)
    //Display("**** for debug rd_val= %x\n",i2c_regs_ptr->ic_data_cmd);  //for debug
                                       // Bit[7:0]: Data  
}

// This is an interrupt service routine (ISR) specific for this testcase
// Upon interrupt (I2C Txfifo empty interrupt),
// 1) write to known location in AHB_SRAM
// 2) clear interrupt
int InterruptServiceRoutine () {

  unsigned WriteData, ReadData, MaskData;

  volatile i2c_regs *i2c_regs_ptr;        // declare a pointer to the I2C regs
  i2c_regs_ptr = (i2c_regs *)I2C_BASE;  // init the pointer 


  // Disable the timer, mask the interrupt
  //-Display("ISR: I2C Interrupt\n");
  WriteData = 0x00000001;                 //Handshake value


  //Address = (unsigned *)(AHB_SRAM_BASE + 0xe0); // init the pointer
  wait_flag = WriteData;                      // Write the handshake value

  // Clear expected I2C interrupt (Txempty)
  ReadData = i2c_regs_ptr->ic_intr_stat;
  //MaskData = ReadData & 0x100;
  MaskData = ReadData & 0x200;
  if (MaskData == 0x200) {
    //  Display("ISR: Clear I2C Activity interrupt");
    //--Display("ISR: Clear I2C Stop detect interrupt\n");
    ReadData = i2c_regs_ptr->ic_clr_stop_det; // 
    return;
  }
  // got an unexpected interrupt
  _FAIL();
}
 
#endif
