#include "timer.h"

void u_delay(unsigned int us)
{

        *TIMER_1_CTRL = 0;

        *TIMER_1_LOAD_VALUE = 0xFFFFFFFF;

        *TIMER_1_CTRL = 1;

	while(1)
	{
		if((0xFFFFFFFF - *TIMER_1_CUR_VALUE) > (us*TICK_PER_USECOND))
		{
			*TIMER_1_CTRL = 0;
			return;
		}
	}	
	
}
