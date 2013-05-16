// General type include
#include "stdtypes.h"

// This module's header file 
#include "ubl.h"

// Device specific CSL
#include "device.h"

// Misc. utility function include
#include "util.h"

// Project specific debug functionality
#include "debug.h"
#include "uartboot.h"


#ifdef UBL_NOR
// NOR driver include
#include "nor.h"
#include "norboot.h"
#endif

#ifdef UBL_NAND
// NAND driver include
#include "nand.h"
#include "nandboot.h"
#endif

#ifdef UBL_SDMMC
// NAND driver include
#include "sdmmc.h"
#include "sdmmcboot.h"
#endif

/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/

static Uint32 LOCAL_boot(void);
static void LOCAL_bootAbort(void);
static void (*APPEntry)(void);


/************************************************************
* Local Variable Definitions                                *
************************************************************/


/************************************************************
* Global Variable Definitions                               *
************************************************************/

Uint32 gEntryPoint;


/************************************************************
* Global Function Definitions                               *
************************************************************/

// Main entry point
void main(void)
{

  // Call to real boot function code
  LOCAL_boot();
    
  // Jump to entry point
//  DEBUG_printString("\r\nJumping to entry point at ");
//  DEBUG_printHexInt(gEntryPoint);
//  DEBUG_printString(".\r\n");
  APPEntry = (void (*)(void)) gEntryPoint;
  (*APPEntry)();  

}


/************************************************************
* Local Function Definitions                                *
************************************************************/

static Uint32 LOCAL_boot(void)
{

  // Platform Initialization
  if ( DEVICE_init() != E_PASS )
  {
    DEBUG_printString(devString);
    DEBUG_printString(" initialization failed!\r\n");
    asm(" MOV PC, #0");
  }
  else
  {
    DEBUG_printString(devString);
    DEBUG_printString(" initialization passed!\r\n");
  }

  // Set RAM pointer to beginning of RAM space
  UTIL_setCurrMemPtr(0);

  // Send some information to host
//  DEBUG_printString("TI UBL Version: ");
//  DEBUG_printString(UBL_VERSION_STRING);
//  DEBUG_printString("\r\nBooting Catalog Boot Loader\r\nBootMode = ");
  
  {
    //Report Bootmode to host
    DEBUG_printString("Attempt UART boot as fallback\r\n");
    UARTBOOT_copy();      
  }

    
//  DEBUG_printString("   DONE");
  
  UTIL_waitLoop(10000);

  DEVICE_TIMER0Stop();

  return E_PASS;    
}

static void LOCAL_bootAbort(void)
{
  DEBUG_printString("Aborting...\r\n");
  while (TRUE);
}

/************************************************************
* End file                                                  *
************************************************************/