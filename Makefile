#############################################################
# Copyright (c) 2013, John Lee <furious_tauren@163.com>
# Sat Jul 19 16:29:45 CST 2014
#############################################################

# ===============================================================
# 1. Global Variables
# ===============================================================
CROSS_COMPILE := arm-none-eabi-
CC = $(CROSS_COMPILE)gcc 
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy

HOSTOS := $(shell uname -o 2>/dev/null | tr [:upper:] [:lower:])
ifeq ($(HOSTOS),cygwin)
TOPDIR := $(shell cygpath -m $(shell pwd))
else
TOPDIR := $(shell pwd)
endif

export CC TOPDIR

# ===============================================================
# 2. CFLAGS
# ===============================================================
# -nostdinc: Do not search the standard system directories
# for header files. Only the directories you have specified
# with -I options(and the directory of the current file, if
# appropriate) are searched.
#
# data-sections: Place each function or data item into its own
# section in the output file. this could cause ld script
# bugs(*(.bss) would be *(.bss*)). 
#
# -isystem $(TOPDIR): variables used in a rule could not contain
#  any colon in make-3.82 on cygwin. so we use relative path.
#
# -std=gnu99 -fgnu89-inline: not only support gnu c99
#  extensions(like asm), but also support c89 inline functions.
# ===============================================================
CFLAGS := -g -fno-common -ffixed-r8
CFLAGS += -fno-builtin -ffreestanding -nostdinc -isystem $(TOPDIR) -pipe
CFLAGS += -marm -mno-thumb-interwork -march=armv7-a -mno-unaligned-access
CFLAGS += -mfloat-abi=softfp -mfpu=neon
CFLAGS += -Wall -Wstrict-prototypes -fno-stack-protector
CFLAGS += -std=gnu99 -fgnu89-inline 

# ----------------------------------------------------------
# User config section
#
# set to 1 to enable AMP (default is single core)
CONFIG_AMP ?= 
# ----------------------------------------------------------
ifneq ($(CONFIG_AMP),)
CFLAGS += -DCONFIG_AMP
endif

# --- macros from A+ --- #
# ---------------------- #
SPECS := ACTIVE_LANGUAGE_LIMIT
SPECS += HEAD_LEVEL_SENSOR
SPECS += TRACE_LEVEL=0
SPECS += SUPPORT_HEAD_HEATER
SPECS += RICOH_VSD2

# SPECS += noRIPSTAR_FLAT_EX	# the square is fixed

# --- board specification
SPECS += IIC_Key_Board_LCD
SPECS += EPSON_LCD
SPECS += LCD_MODULE_ZX19264M1A

SPECS += HEAD_RICOH_G4
SPECS += RICOH_G5_4H

SPECS += EPSON_BOTTOM_BOARD_V2	# a new one

SPECS += FPGA_MAINTAIN_COOR
SPECS += RICOH_CLEAN_PRESS	# big headboard clean manually

SPECS += SUPPORT_MOTOR_CONTROL
SPECS += OPEN_IDLE_FLUSH
SPECS += MANUFACTURER_ORIC_COMMON	# a new one

SPECS += CONVERSION_BOARD
CFLAGS += $(SPECS:%=-D%)

# ===============================================================
# 3. Objects and Includes
# ===============================================================
DIRS := OS lib
INCS := $(shell find $(DIRS) -name "*.h")
INCS := $(sort $(dir $(INCS)))
INCS += include include/ripstar to to/porting

DIRS := OS lib to
SRCS := $(shell find $(DIRS) -name "*.S" -o -name "*.c")
OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

# ===============================================================
# 4. Third Party
# ===============================================================
# lwip objects
LWIPDIR := $(TOPDIR)/3rdparty/lwip-1.4.1/src

INCS += $(LWIPDIR)/include $(LWIPDIR)/include/ipv4 $(LWIPDIR)/port/include

LWIP_CORE_OBJS := mem.o memp.o netif.o pbuf.o stats.o sys.o tcp.o \
	tcp_in.o raw.o tcp_out.o udp.o init.o def.o timers.o
LWIP_CORE_OBJS := $(addprefix $(LWIPDIR)/core/,$(LWIP_CORE_OBJS))

LWIP_IPV4_OBJS := icmp.o ip.o inet.o ip_addr.o inet_chksum.o ip_frag.o
LWIP_IPV4_OBJS := $(addprefix $(LWIPDIR)/core/ipv4/,$(LWIP_IPV4_OBJS))

LWIP_API_OBJS := api_lib.o api_msg.o tcpip.o err.o sockets.o netbuf.o netdb.o
LWIP_API_OBJS := $(addprefix $(LWIPDIR)/api/,$(LWIP_API_OBJS))

LWIP_PORT_OBJS := sys_arch.o lwip_entry.o ethernetif.o stmmac.o
LWIP_PORT_OBJS := $(addprefix $(LWIPDIR)/port/,$(LWIP_PORT_OBJS))
LWIP_PORT_OBJS += $(LWIPDIR)/netif/etharp.o

LWIP_OBJS := $(LWIP_CORE_OBJS) $(LWIP_API_OBJS) $(LWIP_IPV4_OBJS) $(LWIP_PORT_OBJS)
#OBJS += $(LWIP_OBJS)

# ===============================================================
# 5. Rules
# ===============================================================
all: ucosii.lds ucosii.axf

ucosii.axf: $(OBJS)
	@$(CC) -o ucosii.axf -Xlinker --script=ucosii.lds $(OBJS)	\
		-Xlinker -Map=ucosii.map -Xlinker -lm -Xlinker\
		--gc-sections -nostartfiles
	@$(OBJCOPY) -I elf32-little -O binary $@ ucosii.bin


.PHONY: all clean distclean

%.lds: %.lds.S
	@$(CC) $(CFLAGS) $(INCS:%='-I%') -P -E $< -o $@ 

%.o: %.S Makefile
	@$(CC) -D__ASSEMBLY__ $(CFLAGS) $(INCS:%='-I%') -MD -c $< -o $@

%.o: %.c Makefile
	@$(CC) $(CFLAGS) $(INCS:%='-I%') -MD -c $< -o $@


deps := $(OBJS:.o=.d)

clean:
	@rm -f *.axf *.bin *.lds *.map $(OBJS) $(deps)

distclean:
	@rm -f *.axf *.bin *.lds *.map
	@find . -name "*.o" -exec rm -f {} \;
	@find . -name "*.d" -exec rm -f {} \;

# ===============================================================
# 6. Deps
# ===============================================================
-include $(deps)

