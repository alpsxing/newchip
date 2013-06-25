#*****************************************************************************
#
# MAKEFILE - Builds the download utility.
#
#*****************************************************************************

#
# Choose the tools to use based on the host OS.
#
ifeq ("$(OS)","Win32")
NATIVECC = msvc
PROGEXT=.exe
else
NATIVECC = gcc
endif

.PHONY: default 
default: download$(PROGEXT) 

#
# A rule to build the download utility.
#
download$(PROGEXT): download.c 
ifeq ("$(OS)","Windows_NT")
	$(NATIVECC) -DHAVE_STRING_H -o download$(PROGEXT) download.c getopt.c
else
	$(NATIVECC) -o download$(PROGEXT) download.c
endif

mk_crc: mk_crc.c 
	$(NATIVECC) -o mk_crc mk_crc.c


#
# A rule to remove the build products.
#
clean::
	@rm -f download$(PROGEXT) mk_crc
