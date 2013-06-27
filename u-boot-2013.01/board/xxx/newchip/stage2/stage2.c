#include "config.h"
#include "common.h"
#include "arm_defines.h"

void main(void)
{
	unsigned int rom_boot = 1;

	uart_init(115200);
#ifdef MINI_TEST
	uart_puts("UR0431A 0.1");
#else
	printf("UR0431A 0.1\n");

	bspi_test();	
	DDR2_init_Test();
	mem_test(DDR2_DATA_BASE,0x100000);


	rom_boot = (readl(CFG_BOOT_MODE) &0x3c) >> 2 ;
	switch (rom_boot) {
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
	        spi_boot();
			
	    //if spi_boot success, it should never return; 
	    //if it returns, we should try UART, so no break here
        case 0xf:
	        uart_boot();
        default:
            break;
	}
	uart_boot();
#endif
}
