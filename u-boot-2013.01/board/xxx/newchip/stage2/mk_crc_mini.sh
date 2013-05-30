gcc -o mk_crc mk_crc.c board.c -Duart_printf=printf
./mk_crc ./stage2_newchip_mini.bin --offset 0x18 --entry 0x08002800 -o stage2_mini.img
