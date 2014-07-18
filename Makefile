#############################################################
# Copyright (c) 2013, John Lee <furious_tauren@163.com>
# Fri Mar 21 14:25:28 CST 2014
#############################################################

CROSS_COMPILE := arm-none-eabi-
CC = $(CROSS_COMPILE)gcc 
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy

gccincdir := $(shell $(CC) -print-file-name=include)
libgcc_a := $(shell $(CC) -print-libgcc-file-name)

G_FLAGS := -g -fno-common -ffixed-r8
G_FLAGS += -fno-builtin -ffreestanding -nostdinc -isystem $(gccincdir) -pipe
G_FLAGS += -marm -mno-thumb-interwork -march=armv7-a -mno-unaligned-access
G_FLAGS += -mfloat-abi=softfp -mfpu=neon
# =================================================================
# -nostdinc: Do not search the standard system directories
# for header files. Only the directories you have specified
# with -I options(and the directory of the current file, if
# appropriate) are searched.
#
# data-sections: Place each function or data item into its own
# section in the output file. this could cause ld script
# bugs(*(.bss) would be *(.bss*)). 
# =================================================================

ifeq ($(PLATFORM),)
SPECS := ACTIVE_LANGUAGE_LIMIT
SPECS += EPSON_LCD
SPECS += LCD_MODULE_ZX19264M1A
SPECS += HEAD_LEVEL_SENSOR
SPECS += TRACE_LEVEL=0
SPECS += SUPPORT_HEAD_HEATER
SPECS += RICOH_VSD2
SPECS += UV_PRINTER
SPECS += WAVE_SET_FUN
SPECS += RIPSTAR_FLAT_EX OLD_UV
SPECS += COORD_NEW_UV		# confirm

# -----------------------------
# board specification
# -----------------------------
SPECS += IIC_Key_Board_LCD

SPECS += HEAD_RICOH_G4
SPECS += RICOH_G5_4H

SPECS += EPSON_BOTTOM_BOARD_V2	# a new one
# -----------------------------

SPECS += FPGA_MAINTAIN_COOR
SPECS += RICOH_CLEAN_PRESS	# big headboard clean manually

#SPECS += SUPPORT_MOTOR_CONTROL
SPECS += MANUFACTURER_DYSS	# a new one
G_FLAGS += $(SPECS:%=-D%)
endif

CFLAGS := $(G_FLAGS) -Wall -Wstrict-prototypes -fno-stack-protector
CFLAGS += -std=gnu99 -fgnu89-inline 
############################################################
# User config section
#
# set to 1 to enable AMP (default is single core)
CONFIG_AMP ?= 
#############################################################
ifneq ($(CONFIG_AMP),)
CFLAGS += -DCONFIG_AMP
endif

ASFLAGS := -D__ASSEMBLY__ $(CFLAGS)

TOPDIR := $(shell pwd)
export CC CFLAGS TOPDIR

#####################################################################
# All objects
#   Add what you want
#####################################################################
HERDERS := $(shell find $(TOPDIR)/OS -name "*.h")
HERDERS += $(shell find $(TOPDIR)/lib -name "*.h")
INCDIRS := $(dir $(HERDERS))
INCDIRS := $(sort $(INCDIRS))
INCDIRS += $(TOPDIR)/include $(TOPDIR)/include/Ripstar 
INCDIRS += $(TOPDIR)/APP $(TOPDIR)/to $(TOPDIR)/to/porting

CS_DIRS := $(TOPDIR)/APP $(TOPDIR)/OS $(TOPDIR)/lib $(TOPDIR)/to
CS_SRCS := $(shell find $(CS_DIRS) -name "*.S" -o -name "*.c")
CS_OBJS := $(CS_SRCS:%.c=%.o)
CS_OBJS := $(CS_OBJS:%.S=%.o)

# lwip objects
LWIPDIR := $(TOPDIR)/3rdparty/lwip-1.4.1/src

INCDIRS += $(LWIPDIR)/include $(LWIPDIR)/include/ipv4 $(LWIPDIR)/port/include

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
#CS_OBJS += $(LWIP_OBJS)

#####################################################################
# All objects end
##################################################################### 

# support both linux and cygwin env
OBJS := $(CS_OBJS)
ifeq ($(shell uname -o),Cygwin)
	CS_OBJS := $(shell echo $(CS_OBJS)|sed -e 's!/cygdrive/\([a-zA-Z]\)!\1:!g')
	CS_OBJS := $(shell echo $(CS_OBJS)|sed -e 's!/!\\\!g')
	INCDIRS := $(shell echo $(INCDIRS)|sed -e 's!/cygdrive/\([a-zA-Z]\{1\}\)!\1:!g')
	INCDIRS := $(shell echo $(INCDIRS)|sed -e 's!/!\\\!g')
endif

all: ucosii.lds ucosii.axf

ucosii.axf: $(CS_OBJS)
	@$(CC) -o ucosii.axf -Xlinker --script=ucosii.lds $(CS_OBJS)	\
		-Xlinker -Map=ucosii.map -Xlinker -lm -Xlinker\
		--gc-sections -nostartfiles
	@$(OBJCOPY) -I elf32-little -O binary $@ ucosii.bin


.PHONY: all clean

%.lds: %.lds.S
	@$(CC) $(ASFLAGS) $(INCDIRS:%='-I%') -P -E -o $@ $< 

%.o: %.S
	@$(CC) $(ASFLAGS) $(INCDIRS:%='-I%') -MD -c $< -o $@ -g

%.o: %.c
	@$(CC) $(CFLAGS) $(INCDIRS:%='-I%') -MD -c $< -o $@ -g

deps := $(CS_OBJS:%.o=%.d)

clean:
	@rm -f *.axf *.bin *.lds *.map $(OBJS) $(patsubst %.o,%.d,$(OBJS))

-include $(deps)
