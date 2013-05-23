#ifndef DDR2__H
#define DDR2__H

#define  DDR2_BASE_ADDR       0x10000000
#define  DENALI_CTL_BASE_ADDR 0x80030000
#define  PHY_BASE_ADDR          128
#define  PHY_AC_BASE_ADDR       160

#define START_ADDR   0
#define INT_STATUS_ADDR    32

#include "regconfig.h.sim_CTL_125_cl6_bl4_bypass"
//#include "regconfig.h.sim_PHY_125_cl6_bl4_bypass"

#include "regconfig.h.sim_CTL_400_cl6_bl4_bypass"
#include "regconfig.h.sim_PHY_400_cl6_bl4_bypass"

#define WriteRegCTL(addr,data) \
  ArmWrite(DENALI_CTL_BASE_ADDR+((addr)*4),(data))

#define ReadRegCTL(addr) \
  ArmRead(DENALI_CTL_BASE_ADDR+((addr)*4))

#define WriteRegPHY(addr,data) \
  ArmWrite(DENALI_CTL_BASE_ADDR+(PHY_BASE_ADDR)*4+((addr)*4),(data))

#define ReadRegPHY(addr) \
  ArmRead(DENALI_CTL_BASE_ADDR+(PHY_BASE_ADDR)*4+((addr)*4))

void ArmWrite(unsigned int addr, unsigned int data) {
    *(volatile unsigned int *) (addr) = data;
}

unsigned int ArmRead(unsigned int addr) {
    unsigned int read_data = * (volatile unsigned int *) (addr);
    return read_data;
}


#endif

