/*
 *  Desc: Linker command file ubl application program.
 *  Auth: Daniel Allred
 */

ENTRY(boot)
SECTIONS 
{  
  SRAMStart  = 0x08000000;
  SRAMSize   = 0x00004000;
  
  DDRStart   = 0x10000000;
  DDRSize    = 0x10000000;
  EXTERNAL_RAM_START = 0x10000000;
  EXTERNAL_RAM_END   = EXTERNAL_RAM_START + DDRSize;  
  
  STACKStart = SRAMStart + SRAMSize;
  
  /* The entry point address is fixed at 0x100 */
  . = SRAMStart + 0x800;
  .boot :   
  {
    *(.boot)
    . = ALIGN(4);
  }
  .text :   
  {
    *(.text)
    . = ALIGN(4);
  }
  
  .data : 
  {
    *(.data)
    . = ALIGN(4);
  }
      
  .bss    :
  {
    *(.bss) *(COMMON)
    . = ALIGN(4);
  }
}

