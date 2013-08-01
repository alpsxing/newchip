/*
 * Common hardware definitions
 *
 * Author: Kevin Hilman, MontaVista Software, Inc. <source@mvista.com>
 *
 * 2007 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

// Memory Map base definitions
#define ROM_BASE       0x00000000  
#define BSPI_BASE      0x04000000  
#define AHB_SRAM_BASE  0x08000000  
#define DDR2_DATA_BASE 0x10000000  
#define AHB_ARB_BASE   0x80000000  
#define DMAC_BASE      0x80010000  
#define MAC_BASE       0x80020000  
#define DDR2_REG_BASE  0x80030000  
#define HINOC_REG_BASE 0x80040000  
//#define SCU_BASE       0x90000000
#define TIMER_BASE     0x90010000
#define UART0_BASE     0x90020000
#define UART1_BASE     0x90030000
#define WDT_BASE       0x90040000
#define I2C_BASE       0x90050000
#define GPIO0_BASE     0x90060000
#define GPIO1_BASE     0x90070000
#define GPIO2_BASE     0x90080000
#define GPIO3_BASE     0x90090000
#define INTC_BASE      0x900a0000
#define SSI_BASE       0x900b0000
#define DMAC_CHAN_REG_OFFSET     0x00000058
#define DMAC_INTR_REG_OFFSET     0x000002c0
#define DMAC_SW_HSHK_REG_OFFSET  0x00000368
#define DMAC_MISC_REG_OFFSET     0x00000398
#define CFG_BOOT_MODE (0x90000024)

// Defines for data manipulations
#define CFG_APB_CLOCK 50000000
#define CFG_SPI_CLOCK 5000000

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


// UART regs structure
typedef struct {
  unsigned rbr_thr_dlr;
  unsigned dmr_ier;  
  unsigned iir_fcr;
  unsigned lcr;
  unsigned mcr;
  unsigned lsr;
  unsigned msr;
  unsigned sr;
} uart_regs;

// I2C regs structure
typedef struct {
  unsigned ic_con;                //0x0
  unsigned ic_tar;  
  unsigned ic_sar;
  unsigned ic_hs_maddr;
  unsigned ic_data_cmd;           //0x10
  unsigned ic_ss_scl_hcnt;
  unsigned ic_ss_scl_lcnt;
  unsigned ic_fs_scl_hcnt;
  unsigned ic_fs_scl_lcnt;        //0x20
  unsigned ic_hs_scl_hcnt;
  unsigned ic_hs_scl_lcnt;
  unsigned ic_intr_stat;
  unsigned ic_intr_mask;       //0x30
  unsigned ic_raw_intr_stat;
  unsigned ic_rx_tl;
  unsigned ic_tx_tl;
  unsigned ic_clr_intr;        //0x40
  unsigned ic_clr_rx_under;
  unsigned ic_clr_rx_over;
  unsigned ic_clr_tx_over;
  unsigned ic_clr_rd_req;     //0x50
  unsigned ic_clr_tx_abrt;
  unsigned ic_clr_rx_done;
  unsigned ic_clr_activity;
  unsigned ic_clr_stop_det;   //0x60
  unsigned ic_clr_start_det;
  unsigned ic_clr_gen_call;
  unsigned ic_enable;
  unsigned ic_status;          //0x70 
  unsigned ic_txflr;
  unsigned ic_rxflr;
  unsigned ic_sda_hold;
  unsigned ic_tx_abrt_source;   //0x80
  unsigned ic_slv_data_nack_only;
  unsigned ic_dma_cr;
  unsigned ic_dma_tdlr;
  unsigned ic_dma_rdlr;         //0x90
  unsigned ic_sda_setup;
  unsigned ic_ack_general_call;
  unsigned ic_enable_status;
  unsigned ic_fs_spklen;        //0xa0
  unsigned ic_hs_spklen;        //0xa4
} i2c_regs;

// GPIO regs structure
typedef struct {
  unsigned PortAData;  //x0
  unsigned PortADir;
  unsigned PortACtl;
  unsigned PortBData;
  unsigned PortBDir;   //x10
  unsigned PortBCtl;
  unsigned PortCData;
  unsigned PortCDir;
  unsigned PortCCtl;   //x20
  unsigned PortDData;
  unsigned PortDDir;
  unsigned PortDCtl;
  unsigned IntEn;      //x30
  unsigned IntMask;
  unsigned IntLevel;
  unsigned IntPolarity;
  unsigned IntStatus;  //x40
  unsigned RawIntStatus;
  unsigned Debounce;
  unsigned PortAEoi;
  unsigned ExtPortA;   //x50
  unsigned ExtPortB;
  unsigned ExtPortC;
  unsigned ExtPortD;
} gpio_regs;

// Timers regs structure
typedef struct {
  unsigned LoadCount1;  //x0
  unsigned CurrentValue1;
  unsigned Control1;
  unsigned EOI1;
  unsigned IntStatus1;   //x10
  unsigned LoadCount2;   //x14
  unsigned CurrentValue2;
  unsigned Control2;
  unsigned EOI2;
  unsigned IntStatus2;   //x24
  unsigned LoadCount3;  //x28
  unsigned CurrentValue3;
  unsigned Control3;
  unsigned EOI3;
  unsigned IntStatus3;   //38
  unsigned LoadCount4;   //x3c
  unsigned CurrentValue4;
  unsigned Control4;
  unsigned EOI4;
  unsigned IntStatus4;   //x4c
  unsigned PAD1[2];
  unsigned PAD2[16];     //0x58 to x9c
  unsigned IntStatus;    //xa0
  unsigned EOI;          //xa4
  unsigned RawIntStatus;    //xa8
  unsigned Version;      //xac
} timer_regs;

// WDT regs structure
typedef struct {
  unsigned Control;             //0x0
  unsigned TimeoutRange;        //0x04
  unsigned CurrentCounterValue; //0x08
  unsigned CounterRestart;      //0x0C
  unsigned IntStatus;           //0x10
  unsigned EOI;                 //0x14
  unsigned PAD3[2];
  unsigned PAD4[49];            //0x20 to 0xe0
  unsigned Params5;     
  unsigned Params4;
  unsigned Params3;
  unsigned Params2;             //0xF0
  unsigned Params1;             //0xF4
  unsigned Version;             //0xF8
  unsigned Type;                //0xFC
} wdt_regs;

// Interrupt controller regs structure
typedef struct {
  unsigned IntEnL;     //x0
  unsigned IntEnH;
  unsigned IntMaskL;
  unsigned IntMaskH;
  unsigned IntForceL;   //x10
  unsigned IntForceH;   
  unsigned RawStatusL;
  unsigned RawStatusH;
  unsigned StatusL;     //0x20
  unsigned StatusH;
  unsigned MaskStatusL;
  unsigned MaskStatusH;
  unsigned FinalStatusL; //0x30
  unsigned FinalStatusH;
  unsigned irq_vector;  // 0x38
  unsigned IRQ_PAD0;
  unsigned irq_vector_0;// 0x40
  unsigned IRQ_PAD1;
  unsigned irq_vector_1;
  unsigned IRQ_PAD2;
  unsigned irq_vector_2;
  unsigned IRQ_PAD3;
  unsigned irq_vector_3;
  unsigned IRQ_PAD4;
  unsigned irq_vector_4;
  unsigned IRQ_PAD5;
  unsigned irq_vector_5;
  unsigned IRQ_PAD6;
  unsigned irq_vector_6;
  unsigned IRQ_PAD7;
  unsigned irq_vector_7;
  unsigned IRQ_PAD8;
  unsigned irq_vector_8;
  unsigned IRQ_PAD9;
  unsigned irq_vector_9;
  unsigned IRQ_PAD10;
  unsigned irq_vector_10;
  unsigned IRQ_PAD11;
  unsigned irq_vector_11;
  unsigned IRQ_PAD12;
  unsigned irq_vector_12;
  unsigned IRQ_PAD13;
  unsigned irq_vector_13;
  unsigned IRQ_PAD14;
  unsigned irq_vector_14;
  unsigned IRQ_PAD15;
  unsigned irq_vector_15;
  unsigned IRQ_PAD16;
  unsigned fiq_inten;   // 0xC0
  unsigned fiq_intmask;
  unsigned fiq_intforce;
  unsigned fiq_rawstatus;
  unsigned fiq_status;
  unsigned fiq_finalstatus;
  unsigned irq_plevel;  // 0xD8
  unsigned irq_internal_plevel;
  unsigned IRQ_PAD17;
  unsigned IRQ_PAD18;
  unsigned irq_pr[32];  // 0xE8
  unsigned IRQ_PAD19[162];
  unsigned ICTL_COMP_PARAMS_2;  // 0x3F0
  unsigned ICTL_COMP_PARAMS_1;  // 0x3F4
  unsigned AHB_ICTL_COMP_VERSION;
  unsigned ICTL_COMP_TYPE;      // 0x3FC
} intc_regs;

typedef struct {
  unsigned ctrlr0;     //x0
  unsigned ctrlr1;
  unsigned ssienr;
  unsigned mwcr;
  unsigned ser;
  unsigned baudr;
  unsigned txftlr;
  unsigned rxftlr;
  unsigned txflr;
  unsigned rxflr;
  unsigned sr;
  unsigned imr;
  unsigned isr;
  unsigned risr;
  unsigned txoicr;
  unsigned rxoicr;
  unsigned rxuicr;
  unsigned msticr;
  unsigned icr;
  unsigned dmacr;
  unsigned dmatdlr;
  unsigned dmardlr;
  unsigned idr;
  unsigned ssi_comp_version;
  unsigned dr;
  unsigned rx_simple_dly;
  unsigned rsvd_0;
  unsigned rsvd_1;
  unsigned rsvd_2;
} ssi_regs;

//DMA Registers are distrubuted in 4 sets
// - DMA Channel Configuration  (7 * DMAC_CHAN_REG_OFFSET)
// - DMA Interrupt   (DMAC_INTR_REG_OFFSET)
// - DMA SW Handshake  (DMAC_SW_HSHK_REG_OFFSET)
// - DMA Miscellaneous  (DMAC_MISC_REG_OFFSET)
typedef struct {
  unsigned long sar0;
  unsigned long sar0_ub;
  unsigned long dar0;
  unsigned long dar0_ub;
  unsigned long llp0;
  unsigned long llp0_ub;
  unsigned long ctl0;
  unsigned long ctl0_ub;
  unsigned long sstat0;
  unsigned long sstat0_ub;
  unsigned long dstat0;
  unsigned long dstat0_ub;
  unsigned long sstatar0;
  unsigned long sstatar0_ub;
  unsigned long dstatar0;
  unsigned long dstatar0_ub;
  unsigned long cfg0;
  unsigned long cfg0_ub;
  unsigned long sgr0;
  unsigned long sgr0_ub;
  unsigned long dsr0;
  unsigned long dsr0_ub;
} dmac_ch_regs ;

typedef struct {
  unsigned long rawtfr;
  unsigned long rawtfr_ub;
  unsigned long rawblock;
  unsigned long rawblock_ub;
  unsigned long rawsrctran;
  unsigned long rawsrctran_ub;
  unsigned long rawdsttran;
  unsigned long rawdsttran_ub;
  unsigned long rawerr;
  unsigned long rawerr_ub;
  
  unsigned long statustfr;
  unsigned long statustfr_ub;
  unsigned long statusblock;
  unsigned long statusblock_ub;
  unsigned long statussrctran;
  unsigned long statussrctran_ub;
  unsigned long statusdsttran;
  unsigned long statusdsttran_ub;
  unsigned long statuserr;
  unsigned long statuserr_ub;

  unsigned long masktfr;
  unsigned long masktfr_ub;
  unsigned long maskblock;
  unsigned long maskblock_ub;
  unsigned long masksrctran;
  unsigned long masksrctran_ub;
  unsigned long maskdsttran;
  unsigned long maskdsttran_ub ;
  unsigned long maskerr;
  unsigned long maskerr_ub;

  unsigned long cleartfr;
  unsigned long cleartfr_ub;
  unsigned long clearblock;
  unsigned long clearblock_ub;
  unsigned long clearsrctran;
  unsigned long clearsrctran_ub;
  unsigned long cleardsttran;
  unsigned long cleardsttran_ub;
  unsigned long clearerr;
  unsigned long clearerr_ub;
  unsigned long statusint;
  unsigned long statusint_ub;
} dmac_intr_regs;

typedef struct { 
  unsigned long reqsrcreg;
  unsigned long reqsrcreg_ub;
  unsigned long reqdstreg;
  unsigned long reqdstreg_ub;
  unsigned long sglreqsrcreg;
  unsigned long sglreqsrcreg_ub;
  unsigned long sglreqdstreg;
  unsigned long sglreqdstreg_ub;
  unsigned long lstsrcreg;
  unsigned long lstsrcreg_ub;
  unsigned long lstdstreg;
} dmac_sw_hshk_regs;
  
typedef struct {
  unsigned long dmacfgreg;
  unsigned long dmacfgreg_ub;
  unsigned long chenreg;
  unsigned long chenreg_ub;
  unsigned long dmaidreg;
  unsigned long dmaidreg_ub;
  unsigned long dmatestreg;
  unsigned long dmatestreg_ub;
  unsigned long rsv_3b8;
  unsigned long rsv_3b8_ub;
  unsigned long rsv_3c0;
  unsigned long rsv_3c0_ub;
  unsigned long dma_comp_param_6;
  unsigned long dma_comp_param_6_ub;
  unsigned long dma_comp_param_5;
  unsigned long dma_comp_param_5_ub;
  unsigned long dma_comp_param_4;
  unsigned long dma_comp_param_4_ub;
  unsigned long dma_comp_param_3;
  unsigned long dma_comp_param_3_ub;
  unsigned long dma_comp_param_2;
  unsigned long dma_comp_param_2_ub;
  unsigned long dma_comp_param_1;
  unsigned long dma_comp_param_1_ub;
} dmac_misc_regs;



//************************************************************
// This routine will enable any individual interrupt, or all of them.
// Specific for the interrupt controller being used, DW_ahb_ictl
// Interrupts are as follows:
// IRQ0 : UART0
// IRQ1 : UART1
// IRQ2 : I2C                                                     
// IRQ3 : GPIO0
// IRQ4 : GPIO1                                                     
// IRQ5 : GPIO2                                                     
// IRQ6 : GPIO3                                                     
// IRQ7 : TIMER0/Intr0                                                     
// IRQ8 : TIMER0/Intr1                                                     
// IRQ9 : TIMER0/Intr2                                                     
// IRQ10 : IIMTER0/Intr3                                                     
// IRQ11 : SSI                                                     
// IRQ12 : DMAC
// IRQ13 : WDT                                                     
//
// FIQ0 : HINOC_int[0]                                                     
// FIQ1 : HINOC_int[1]                                                    
// FIQ2 : HINOC_int[2]                                                     
// FIQ3 : HINOC_int[3]                                                     
// FIQ4 : DDR2
// FIQ5 : AHB
// FIQ6 : GMAC
//************************************************************
#if 0
int InterruptEnable (unsigned IntNum) {  // {{{

  volatile intc_regs *intc_regs_ptr;      // declare a pointer to the Intc regs
  intc_regs_ptr = (intc_regs *)INTC_BASE; // init the pointer


  if (IntNum == 0) { 
    //Display("InterruptEnable: enabling all interrupts");
    // Unmask all interrupts 
    intc_regs_ptr->IntMaskL = 0x0; // unmask bits
    // Enable interrupt  
    intc_regs_ptr->IntEnL = 0xffff; // enable bit
  }
  else {
    //Display("InterruptEnable: enabling interrupt %r8", IntNum);
    // Unmask interrupt 
    intc_regs_ptr->IntMaskL = ~IntNum; // unmask bits
    // Enable interrupt  
    intc_regs_ptr->IntEnL = IntNum; // enable bit
  }
  
  return;
}  // InterruptEnable


#endif /* __ASM_ARCH_HARDWARE_H */

#endif
