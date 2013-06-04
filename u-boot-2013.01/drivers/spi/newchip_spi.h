#ifndef _NEWCHIP_SPI_H_
#define _NEWCHIP_SPI_H_

// BSPI regs structure
typedef struct {
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
} bspi_regs;

#define BIT(x)			(1 << (x))

struct newchip_spi_slave {
	struct spi_slave slave;
	volatile bspi_regs *regs;
	unsigned int freq;
};

static inline struct newchip_spi_slave *to_newchip_spi(struct spi_slave *slave)
{
	return container_of(slave, struct newchip_spi_slave, slave);
}

#endif /* _DAVINCI_SPI_H_ */
