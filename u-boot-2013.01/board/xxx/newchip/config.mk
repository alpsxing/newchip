# Physical Address: 20000 to 802'0000
#
# Linux Kernel is expected to be at 0000'8000, entry 0000'8000
# (mem base + reserved)
#

#Provide at least 16MB spacing between us and the Linux Kernel image
CONFIG_SYS_TEXT_BASE = 0x11100000
