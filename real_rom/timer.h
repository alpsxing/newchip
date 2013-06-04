#ifndef _UR0431A_TIMER_H_

#define _UR0431A_TIMER_H_

#include "config.h"


#define TICK_PER_USECOND (CFG_APB_CLOCK/1000000)

#define TIMER_1_LOAD_VALUE ((volatile unsigned int*)(TIMER_BASE_ADDR + 0x00))
#define TIMER_1_CUR_VALUE ((volatile unsigned int*)(TIMER_BASE_ADDR + 0x04))
#define TIMER_1_CTRL ((volatile unsigned int*)(TIMER_BASE_ADDR + 0x8))
#define TIMER_1_INT_STATUS ((volatile unsigned int*)(TIMER_BASE_ADDR + 0x10))

//functions============================
void u_delay(unsigned int us);
#endif



