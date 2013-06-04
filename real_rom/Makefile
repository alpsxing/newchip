SHELL = /bin/sh

# Set pointer to ARM binutils and gcc
export ARM_BIN := /spse/prj/UR0431A/design/bin/gnuarm-4.5.0/bin

ARM_TEST = real_rom
##################################################
#
# Variables related to assembling arm926 test programs
#
##################################################
GCC_ARM926  = arm-elf-gcc -mcpu=arm926ej-s -mfpu=vfp -mfloat-abi=softfp -static 
GCC_ARM926  += -I ../include
GCC_ARM926  += -I ../../../../../../sdvp/common/C/
GCC_ARM926  += -std=gnu99
LD          = arm-elf-ld
OBJDUMP     = arm-elf-objdump --syms --reloc --disassembler-options=reg-names-std
OBJCOPY     = arm-elf-objcopy -O srec -R .comment -R .debug_abbrev -R .debug_line -R .debug_loc -R .debug_pubnames -R .debug_pubtypes -R .debug_str -R .debug_info -R .debug_frame -R .debug_macinfo{OBJCOPY}
READELF     = arm-elf-readelf


ARCHIVE1 = ${ARM_BIN}/../arm-elf/lib
ARCHIVE2 = ${ARM_BIN}/../lib/gcc/arm-elf/4.5.0
#GCC_LIBS = -L${ARCHIVE1} -L$(ARCHIVE2) -lg -lm -lc -lgcc
GCC_LIBS = -L${ARCHIVE1} -L$(ARCHIVE2) -lm -lc -nostdlibs 
#GCC_LIBS = -L${ARCHIVE1} -L$(ARCHIVE2) -lm -lc 

# define assembler options, and provide a way to modify it on a testcase basis
AS_ARM926  += arm-elf-as -mcpu=arm926ej-s -mfpu=vfp -mfloat-abi=softfp
#AS_ARM926  += arm-elf-as -mcpu=arm926ej-s 
AS_ARM926  += -I .
AS_ARM926  += --defsym ASM_ARM926=1 
AS_ARM926  += ${AS_A926_OPT}

%.o:%.c
	${GCC_ARM926} -O -c $< -o $@

run: vector_arm926.s startup_arm926.s _udivsi3.S _umodsi3.S _divsi3.S uart.o spi.o string.o printf.o timer.o div0.o real_rom.o
	# input C source (.c) in c_files, output obj in run
	echo "Compiling ASM source: ${AS_ARM926} vector_arm926.s"
	${AS_ARM926} vector_arm926.s -o ./vector_arm926.o
	echo "Compiling ASM source: ${AS_ARM926} startup_arm926.s"
	${AS_ARM926} startup_arm926.s -o ./startup_arm926.o
	echo "Compiling ASM source: ${AS_ARM926} _divsi3.S"
	${AS_ARM926} _divsi3.S  -o ./_divsi3.o
	${AS_ARM926} _udivsi3.S -o ./_udivsi3.o
	${AS_ARM926} _umodsi3.S -o ./_umodsi.o
	echo "Linking C test, and ASM startup..."
	$(LD) -T link.map ${GCC_LIBS} *.o -o  $(ARM_TEST).elf
	echo "Files linked into $(ARM_TEST).elf..."
	# input obj in run, output srec in run
	echo "Copying to srec format: $(ARM_TEST).srec"
	${OBJCOPY} $(ARM_TEST).elf $(ARM_TEST).srec
	# create the target, disassembler in run
	echo "generating disassembler listing for debug: $(ARM_TEST).dis"
	${OBJDUMP} $(ARM_TEST).elf --disassemble-all > $(ARM_TEST).dis	
	echo "generating header file: $(ARM_TEST).headers.txt"
	${READELF} -S $(ARM_TEST).elf > $(ARM_TEST).headers.txt
	@# input srec in run, output mif data file in run
	@#echo "Converting to DW Memory Model mif format: $(ARM_TEST).srec"
	@#$(DESIGNWARE_HOME)/bin/arch/cnvrt2mif/latest/linux/cnvrt2mif $(ARM_TEST).srec -devwidth 32 -devs 1 -flip32 -o $(ARM_TEST)
	@#/spse/prj/UR0431A/design/incoming/Uboot/mif2vhx.pl -i $(ARM_TEST).mif
	@#/spse/prj/UR0431A/design/incoming/Uboot/img2vhx_flash.exe > init_flash.vhx
	@#echo "Converting to Altera mif format: altera_rom.mif"
	@#cat altera_rom.mif_header > altera_rom.mif
	@#sed -e 's/\//:/' $(ARM_TEST).mif >> altera_rom.mif
	@#echo "END;" >> altera_rom.mif

clean:
	rm -rf *.o *.srec *.dis *.mif *.vhx *.txt *.elf
