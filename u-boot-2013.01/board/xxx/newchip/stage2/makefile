DEVSTRING=newchip
TYPE=uart

PROGRAM:=stage2

CROSSCOMPILE?=arm-none-linux-gnueabi-

CC=$(CROSSCOMPILE)gcc
OBJCOPY=$(CROSSCOMPILE)objcopy
OBJDUMP=$(CROSSCOMPILE)objdump
LDFLAGS=-Wl,-T$(LINKERSCRIPT) -nostdlib  
OBJCOPYFLAGS = --gap-fill 0xFF -S
SOURCES=$(PROGRAM).c boot.c uart.c printf.c string.c timer.c board.c ddr2_init.c bspi_test.c spi.c div0.c
ASSEMBLY= _divsi3.S _udivsi3.S _umodsi3.S
ENTRYPOINT=boot

CFLAGS:=-c -Os -Wall -ffreestanding -I./ 

OBJECTSC:=$(patsubst %.c,%_$(TYPE).o,$(SOURCES)) 
OBJECTS:=$(OBJECTSC) $(patsubst %.c,%_$(TYPE).o,$(ASSEMBLY))
EXECUTABLE:=$(PROGRAM)_$(DEVSTRING)_$(TYPE).elf
BINARY:=./$(PROGRAM)_$(DEVSTRING)_$(TYPE).bin
LINKERSCRIPT=./$(PROGRAM).lds

# Target definitions
all: $(BINARY) $(OBJECTS)

.PHONY : clean objclean
clean:
		-rm -f -v $(OBJECTSC) $(BINARY) $(EXECUTABLE) *.img
		
objclean:
		-rm -f -v $(OBJECTSC)
		
$(BINARY): $(EXECUTABLE)
		$(OBJCOPY) $(OBJCOPYFLAGS) -O binary $< $@
		@echo -n "The entry point of the binary is 0x"
		@$(OBJDUMP) -t $< | grep -E '[0-9a-f]{8}*[0-9a-f]{8} $(ENTRYPOINT)' | sed  's/^\([0-9a-f]\{4\}\)\([0-9a-f]\{4\}\).*/\2/'
		./mk_crc.sh

$(EXECUTABLE): $(OBJECTS) $(LINKERSCRIPT)
		$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
		
%_$(TYPE).o : %.c
		$(CC) $(CFLAGS) $< -o $@      
        
