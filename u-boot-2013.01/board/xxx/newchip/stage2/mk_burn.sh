gcc -o mk_burn mk_burn.c 
./mk_burn ./stage2_newchip_uart.bin --entry 0x10200000 --offset 0x0 --ld 0x08000800 -o stage2_burn.img
