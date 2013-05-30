gcc -o mk_crc mk_crc.c board.c -Duart_printf=printf
./mk_crc ./stage2_newchip_uart.bin --offset 0x18 --entry 0x08000800 -o stage2.img
