
#ifndef __CONFIG_H_
#define __CONFIG_H_

#define CURRENT_DEBUG_LEVEL 3
#define CFG_PBX
//#define COSIM

#define CFG_HEAD_TAG 'NUFX'

//Base & reg
#ifdef CFG_PBX      //setting in PBX
#define CFG_APB_CLOCK 50000000

#define CFG_BOOT_MODE (0x90000024) 

#define UART_0_BASE (0x90020000)
#define SPI_0_BASE  (0x04000000)
#define TIMER_BASE_ADDR (0x90010000)


#define CFG_IRAM_ADDR  0x08000000
#define TEST_BUF_ADDR  0x0201C000
#define TEST_OUT_BUF   0x0201D000
#endif

//memory config
#define CFG_IRAM_SIZE	0x00004000  //16KB

//SCM & PRCM
//X-loader
#define CFG_XLOADER_ADDR	64   //64 sector from 0

// serial defines 
#define CFG_PRINTF
#define CFG_UART_BAUD_RATE 115200
#define CFG_PBSIZE 256

//spi defines
#define CFG_SPI_CLOCK 5000000

#endif /* __CONFIG_H */

