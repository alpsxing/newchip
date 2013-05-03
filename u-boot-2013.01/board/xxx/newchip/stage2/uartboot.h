#ifndef _UARTBOOT_H_
#define _UARTBOOT_H_

#include "stdtypes.h"

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif

/************************************************************
* Global Macro Declarations                                 *
************************************************************/


/************************************************************
* Global Typedef declarations                               *
************************************************************/

typedef struct _UARTBOOT_HEADER_
{
  Uint32      magicNum;
  Uint32      startAddr;
  Uint32      loadAddr;
  Uint32      byteCnt;
  Uint32      crcVal;
  Uint8       *imageBuff;
}
UARTBOOT_HeaderObj,*UARTBOOT_HeaderHandle;


/************************************************************
* Global Function Declarations                              *
************************************************************/

Uint32 UARTBOOT_copy(void);

#endif // End _UARTBOOT_H_

