#ifndef _TISTDTYPES_H_
#define _TISTDTYPES_H_

/*
 * Null declaration 
 */
#ifndef NULL
  #define NULL    (0)
#endif

/*
 * Return type defines 
 */
#ifndef E_PASS
  #define E_PASS    (0x00000000u)
#endif
#ifndef E_FAIL
  #define E_FAIL    (0x00000001u)
#endif
#ifndef E_TIMEOUT
  #define E_TIMEOUT (0x00000002u)
#endif
#ifndef E_DEVICE
  #define E_DEVICE (0x00000003u)
#endif

// Aliases for standard C types
typedef int                     Int;
typedef unsigned                Uns;
typedef char                    Char;
typedef char                    *String;	/* pointer to null-terminated character sequence */
typedef void                    *Ptr;       /* pointer to arbitrary type */
typedef unsigned short          Bool;       /* boolean */

// Boolean declarations
#ifndef TRUE
  #define TRUE    ((Bool) 1)
#endif
#ifndef FALSE
  #define FALSE   ((Bool) 0)
#endif

// GENERIC BUS width defines
#ifndef BUS_8BIT
  #define BUS_8BIT            (0x01)
#endif
#ifndef BUS_16BIT
  #define BUS_16BIT           (0x02)
#endif
#ifndef BUS_32BIT
  #define BUS_32BIT           (0x04)
#endif
#ifndef BUS_64BIT
  #define BUS_64BIT           (0x08)
#endif

// Generic bit mask defines
#ifndef BIT0
  #define BIT0    (0x00000001)
  #define BIT1    (0x00000002)
  #define BIT2    (0x00000004)
  #define BIT3    (0x00000008)
  #define BIT4    (0x00000010)
  #define BIT5    (0x00000020)
  #define BIT6    (0x00000040)
  #define BIT7    (0x00000080)
  #define BIT8    (0x00000100)
  #define BIT9    (0x00000200)
  #define BIT10   (0x00000400)
  #define BIT11   (0x00000800)
  #define BIT12   (0x00001000)
  #define BIT13   (0x00002000)
  #define BIT14   (0x00004000)
  #define BIT15   (0x00008000)
  #define BIT16   (0x00010000)
  #define BIT17   (0x00020000)
  #define BIT18   (0x00040000)
  #define BIT19   (0x00080000)
  #define BIT20   (0x00100000)
  #define BIT21   (0x00200000)
  #define BIT22   (0x00400000)
  #define BIT23   (0x00800000)
  #define BIT24   (0x01000000)
  #define BIT25   (0x02000000)
  #define BIT26   (0x04000000)
  #define BIT27   (0x08000000)
  #define BIT28   (0x10000000)
  #define BIT29   (0x20000000)
  #define BIT30   (0x40000000)
  #define BIT31   (0x80000000)
#endif
  

    /* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */
    typedef unsigned int                 Uint32;
    typedef unsigned short               Uint16;
    typedef unsigned char                Uint8;

    /* Signed integer definitions (32bit, 16bit, 8bit) follow... */
    typedef int                          Int32;
    typedef short                        Int16;
    typedef char                         Int8;

	/* Volatile unsigned integer definitions (32bit, 16bit, 8bit) follow... */
    typedef volatile unsigned int        VUint32;
    typedef volatile unsigned short      VUint16;
    typedef volatile unsigned char       VUint8;

    /* Volatile signed integer definitions (32bit, 16bit, 8bit) follow... */
    typedef volatile int                 VInt32;
    typedef volatile short               VInt16;
    typedef volatile char                VInt8;

    /* empty defines */
    #define __FAR__ 
    #define __NEAR__

#endif  /* _TISTDTYPES_H_ */