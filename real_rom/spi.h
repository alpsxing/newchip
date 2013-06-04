#ifndef __SPI_H_
#define __SPI_H_

struct UR0431A_SPI {
  unsigned spi_go;
  unsigned ctrl0;  
  unsigned ctrl1;
  unsigned ctrl2;
  unsigned Rx0;
  unsigned Rx1;
  unsigned Rx2;
  unsigned Rx3;
  unsigned Tx0;
  unsigned Tx1;
  unsigned Tx2;
  unsigned Tx3;  
};

typedef volatile struct UR0431A_SPI *UR0431A_SPI_t;

#define WP_PAD_HI   0x4000  /*WP pin set 1 */
#define ASS_EN      0x2000  /*slave select auto generate enable */
#define LSB_MODE    0x1000  /*set LSB first mode */
#define MOSI_NEG_EN 0x0400  /*send data use sclk negedge */
#define MISO_NEG_EN 0x0300  /*receive data use sclk negedge */

#define CS0_EN      0x10000 /*controller use cs0 select slave.*/

/*TODO begin: add support on ASIC later*/
#define EXP_SLAVE_ID     0x0117 /*spansion SPI device ID support in FPGA*/
#define READ_ID_CMD      0x90
#define READ_BYTES_CMD   0x03 
#define CMD_LEN          8    /*bit*/ 
#define ID_LEN           16   /*bit*/
#define ADDR_LEN         24   /*bit*/
/*TODO: end*/

#endif
