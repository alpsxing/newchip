#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "stdtypes.h"

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

/***********************************************************
* Global Typedef declarations                              *
***********************************************************/

/***********************************************************
* Global Function Declarations                             *
***********************************************************/

Uint32 DEBUG_printString(String s);
Uint32 DEBUG_printHexInt(Uint32 i);
Uint32 DEBUG_readString(String s);
Uint32 DEBUG_readChar(Char *c);


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_DEBUG_H_

