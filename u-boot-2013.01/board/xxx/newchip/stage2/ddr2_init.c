#include "config.h"
#include "common.h"
#include "DDR2/ddr2.h"

void InitializeDDR(const unsigned int *ctl_cfg, const unsigned int *phy_cfg) {

	unsigned int i;

	TRACE(KERN_DEBUG,"Initialize CTL Register ...\n");
	for(i=0;i<71;i++) {
		WriteRegCTL(i, ctl_cfg[i]);
	}
	TRACE(KERN_DEBUG,"Initialize CTL Register Done\n");

#ifndef FPGA_C
	TRACE(KERN_DEBUG,"Initialize PHY Register ...\n");
	for(i=32;i<40;i++) {
		WriteRegPHY(i, phy_cfg[i]);
	}
	for(i=0;i<32;i++) {
		WriteRegPHY(i, phy_cfg[i]);
	}
	TRACE(KERN_DEBUG,"Initialize PHY Register Done\n");
#endif

	TRACE(KERN_DEBUG,"Assert DDR_CTL start parameter.\n");
	WriteRegCTL( START_ADDR, 0x401);

	TRACE(KERN_DEBUG,"Probe DDR_CTL initialization complete interrupt...\n");
	while( (ReadRegCTL(INT_STATUS_ADDR) & (1<<2)) == 0) {
		u_delay(100);
	}
	TRACE(KERN_DEBUG,"DDR_CTL initialization complete interrupt asserted!\n");

	TRACE(KERN_DEBUG,"MSG:DDR2_Init_Done\n");

}

void DDR2_init_Test(void) {

	unsigned long rd_data, wr_data;
	unsigned int i, bank, col, row, addr;
	const unsigned int *ctl_cfg;
	const unsigned int *phy_cfg;
	long long unsigned pattern[32];
	long long unsigned *rd_phy_val, *tmp_val; 

	TRACE(KERN_DEBUG,"DDR2 Initialization Test BEGIN ...\n");

#ifdef FPGA_C
	TRACE(KERN_DEBUG,"Initialize DDR2 Controller & UniPHY ...\n");
	ctl_cfg = regconfig_CTL_126_cl4_bl4_bypass;
	//  phy_cfg = regconfig_PHY_125_cl6_bl4_bypass;
#else
	TRACE(KERN_DEBUG,"Initialize DDR2 Controller & PHY ...\n");
	ctl_cfg = regconfig_CTL_400_cl6_bl4_bypass;
	phy_cfg = regconfig_PHY_400_cl6_bl4_bypass;
#endif
	InitializeDDR(ctl_cfg, phy_cfg);
	TRACE(KERN_DEBUG,"Initialize DDR2 Controller Done.\n");

	TRACE(KERN_DEBUG,"DDR2 Access Range Test...\n");

	//////////////////////////////////
	//
	//   DDR2 is ready for use
	//   Please insert ddr2 test code below
	//
	///////////////////////////////////



	TRACE(KERN_DEBUG,"DDR2 Access Range Test...Done\n");

}

int memfill32( unsigned int start, unsigned int len, unsigned int val )
{
	unsigned int i;
	unsigned int end = start + len;
	unsigned int errorcount = 0;
	unsigned int *pdata;

	/* Write Pattern */
	pdata = (unsigned int *)start;
	for ( i = start; i < end; i += 4 )
	{
		*pdata++ = val;
	}

	/* Read Pattern */
	pdata = (unsigned int *)start;
	for ( i = start; i < end; i += 4 )
	{
		if ( *pdata++ != val )
		{
			errorcount++;
			break;
		}
	}

	return errorcount;
}

int memaddr32( unsigned int start, unsigned int len )
{
	unsigned int i;
	unsigned int  end = start + len;
	int errorcount = 0;
	unsigned int *pdata;


	/* Write Pattern */
	pdata = (unsigned int *)start;
	for ( i = start; i < end; i += 16 )
	{
		*pdata++ = i;
		*pdata++ = i + 4;
		*pdata++ = i + 8;
		*pdata++ = i + 12;
	}

	/* Read Pattern */
	pdata  = (unsigned int *)start;
	for ( i = start; i < end; i += 4 )
	{
		if ( *pdata++ != i )
		{
			errorcount++;
			break;
		}
	}

	return errorcount;
}

int  meminvaddr32( unsigned int start, unsigned int len )
{
	unsigned int i;
	unsigned int end = start + len;
	int errorcount = 0;
	unsigned int *pdata;

	/* Write Pattern */
	pdata = (unsigned int *)start;
	for ( i = start; i < end; i += 4 )
	{
		*pdata++ = ~i;
	}

	/* Read Pattern */
	pdata = (unsigned int *)start;
	for ( i = start; i < end; i += 4 )
	{
		if ( *pdata++ != ~i )
		{
			errorcount++;
			break;
		}
	}

	return errorcount;
}


int mem_test(int ddr_base, int ddr_size )
{
	int i=0, errors = 0;

	printf( "  > Data test (quick)\n" );
	if ( memfill32( ddr_base, ddr_size, 0xFFFFFFFF ) )
		errors += 1;

	if ( memfill32( ddr_base, ddr_size, 0xAAAAAAAA ) )
		errors += 2;

	if ( memfill32( ddr_base, ddr_size, 0x55555555 ) )
		errors += 4;

	if ( memfill32( ddr_base, ddr_size, 0x00000000 ) )
		errors += 8;

	if ( errors )
		printf( "        > Error = 0x%x\n", errors );




	printf( "  > Addr test (quick)\n    " );
	if ( memaddr32( ddr_base, ddr_size ) )
	{
		printf("(X) ");
		errors += 16;
	}
	printf("\n");

	printf( "  > Inv addr test (quick)\n    " );
	printf("A%d ", i + 16);
	if ( meminvaddr32( ddr_base, ddr_size ) )
	{
		printf("(X) ");
		errors += 16;
	}
	printf("\n");
	if ( errors )
		printf( "        > Error = 0x%x\n", errors );
	return errors;
}

