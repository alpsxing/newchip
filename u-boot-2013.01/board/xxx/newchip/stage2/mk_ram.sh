gcc -o mk_ram mk_ram.c 
./mk_ram ./stage2_newchip_uart.bin --entry 0x08000800 -o stage2_ram.img
