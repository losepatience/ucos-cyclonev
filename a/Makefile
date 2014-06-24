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

CPPFLAGS := -g -fno-common -ffixed-r8
CPPFLAGS += -fno-builtin -ffreestanding -nostdinc -isystem $(gccincdir) -pipe
CPPFLAGS += -marm -mno-thumb-interwork -march=armv7-a -mno-unaligned-access
CPPFLAGS += -mfloat-abi=softfp -mfpu=neon
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

CFLAGS := $(CPPFLAGS) -Wall -Wstrict-prototypes -fno-stack-protector
CFLAGS += -std=gnu99 -fgnu89-inline 

ASFLAGS := -D__ASSEMBLY__ $(CFLAGS)

TOPDIR := $(shell pwd)
export CC CFLAGS TOPDIR

#####################################################################
# All objects
#   Add what you want
#####################################################################
INCDIRS := $(TOPDIR)/include

CS_SRCS := $(shell find $(TOPDIR)/src -name "*.S" -o -name "*.c")
CS_OBJS := $(CS_SRCS:%.c=%.o)
CS_OBJS := $(CS_OBJS:%.S=%.o)

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
