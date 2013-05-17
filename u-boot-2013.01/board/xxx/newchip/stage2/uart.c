// General type include
#include "stdtypes.h"

// This module's header file 
#include "uart.h"

// Device specific CSL
#include "device.h"

// Misc. utility function module
#include "util.h"


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

static Uint32 LOCAL_getStringLen(String seq);


/************************************************************
* Local Variable Definitions                                *
************************************************************/


/************************************************************
* Global Variable Definitions                               *
************************************************************/


/************************************************************
* Global Function Definitions                               *
************************************************************/

#define LCRVAL LCR_8N1					/* 8 data, 1 stop, no parity */
#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR)	/* Clear & enable FIFOs */

#define CFG_APB_CLOCK 50000 
#define UART_0_BASE  0x90020000
#define com_port ((UR0431A_UART_t)UART_0_BASE)	

int uart_init(int baudrate)
{
	unsigned int baud_divisor;
	
	baud_divisor = CFG_APB_CLOCK / 16 / baudrate;	

	if(CFG_APB_CLOCK > (baud_divisor * 16 * baudrate))
		baud_divisor +=((CFG_APB_CLOCK/16 - baud_divisor * baudrate)*2 > baudrate) ? 1 : 0;


	com_port->ier = 0x00;
	com_port->lcr = LCR_BKSE | LCRVAL;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlh = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
	com_port->fcr = FCRVAL;
	
	return 0;
}

void uart_putc(char c)
{

	while ((com_port->lsr & LSR_THRE) == 0);
	if (c == '\n')
	{
		com_port->thr = '\r';	
		while ((com_port->lsr & LSR_THRE) == 0);
		com_port->thr = '\n';
	}
	else	
		com_port->thr = c;

}

void uart_putchar(char c)
{

        while ((com_port->lsr & LSR_THRE) == 0);
                com_port->thr = c;

}

int uart_getc (void)
{
	while ((com_port->lsr & LSR_DR) == 0);
	return (com_port->rbr);
}


// Send specified number of bytes 
Uint32 UART_sendString(String seq, Bool includeNull)
{
  Uint32 status = 0;
  Int32 i,numBytes;
  Uint32 timerStatus = 1;
	
  numBytes = includeNull?(LOCAL_getStringLen(seq)+1):(LOCAL_getStringLen(seq));
	
  for(i=0;i<numBytes;i++)
  {
    // Send byte 
    uart_putchar(seq[i]);
    //(UART0->THR) = seq[i];
  }
  return E_PASS;
}

Uint32 UART_sendHexInt(Uint32 value)
{
  char seq[9];
  Uint32 i,shift,temp;

  for( i = 0; i < 8; i++)
  {
    shift = ((7-i)*4);
    temp = ((value>>shift) & (0x0000000F));
    if (temp > 9)
    {
      temp = temp + 7;
    }
    seq[i] = temp + 48;	
  }
  seq[8] = 0;
  return UART_sendString((String)seq, FALSE);
}

Uint32 UART_recvString(String seq)
{
  Uint32 len = MAXSTRLEN;
  return UART_recvStringN(seq,&len,TRUE);
}

// Receive data from UART 
Uint32 UART_recvStringN(String seq, Uint32* len, Bool stopAtNull)
{
  Uint32 i, status = 0;
  Uint32 timerStatus = 1;
  	
  for(i=0;i<(*len);i++)
  {
    // Receive byte 
    //seq[i] = (UART0->RBR) & 0xFF;
    seq[i] = uart_getc();
    if (stopAtNull && (seq[i] == 0x00))
    {
      *len = i;
      break;
    }
  }
  return E_PASS;
}

// More complex send / receive functions
Uint32 UART_checkSequence(String seq, Bool includeNull)
{
  Uint32 i, numBytes;
  Uint32 status = 0,timerStatus = 1;

  numBytes = includeNull?(LOCAL_getStringLen(seq)+1):(LOCAL_getStringLen(seq));

  for(i=0;i<numBytes;i++)
  {
    if( uart_getc() != seq[i] )
      return E_FAIL;
  }
  return E_PASS;
}

Uint32 UART_recvHexData(Uint32 numBytes, Uint32* data)
{
  Uint32 i,j;
  Uint32 temp[8];
  Uint32 timerStatus = 1, status = 0;
  Uint32 numLongs, numAsciiChar, shift;
    
  if(numBytes == 2)
  {
    numLongs = 1;
    numAsciiChar = 4;
    shift = 12;
  }
  else
  {
    numLongs = numBytes/4;
    numAsciiChar = 8;
    shift = 28;
  }

  for(i=0;i<numLongs;i++)
  {
    data[i] = 0;
    for(j=0;j<numAsciiChar;j++)
    {
      // Converting ascii to Hex
      temp[j] = uart_getc()-48;
      if(temp[j] > 22)    // To support lower case a,b,c,d,e,f
        temp[j] = temp[j] - 39;
      else if(temp[j]>9)  // To support upper case A,B,C,D,E,F
        temp[j] = temp[j] - 7;

      data[i] |= (temp[j]<<(shift-(j*4)));
    }
  }
  return E_PASS;
}


/************************************************************
* Local Function Definitions                                *
************************************************************/

// Get string length by finding null terminating char
static Uint32 LOCAL_getStringLen(String seq)
{
  Uint32 i = 0;
  while ((seq[i] != 0) && (i<MAXSTRLEN)){ i++; }
  if (i == MAXSTRLEN)
    return ((Uint32)-1);
  else
    return i;
}


/***********************************************************
* End file                                                 *
***********************************************************/

/* --------------------------------------------------------------------------
  HISTORY
    v1.00 - DJA - 02-Nov-2007
      Initial release
-------------------------------------------------------------------------- */
