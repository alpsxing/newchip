#include "config.h"
#include "common.h"
#include "arm_defines.h"

void main(void)
{
	uart_init(115200);
	printf("UR0431A 0.1\n");

	bspi_test();	
	DDR2_init_Test();
	mem_test(DDR2_DATA_BASE,0x100000);
	uart_boot();
}
