# ****************************************************************************************
# Machine flags

CC_CMACH	= -mmcu=msp430f5438 -D__MSP430F5438__ -DGCC_MSP430X

# ****************************************************************************************
# Build and linker flags

CFLAGS		+= $(CC_CMACH) $(CC_DMACH)
CFLAGS		+= -fno-force-addr -finline-limit=1 -fno-schedule-insns
CFLAGS		+= -fshort-enums -Wl,-Map=output.map -fdiagnostics-color=always
CFLAGS		+= -Wall -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align \
		-Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes \
		-Wmissing-declarations -Wunused

# Depend flags
CFLAGS		+= -MMD

# MSP430 data model flags
CFLAGS_DMS	+= -D__DATA_MODEL_SMALL__ -msmall
CFLAGS_DML	+= -D__DATA_MODEL_LARGE__ -mlarge

# Specific linker script for each data model
# @note: The small model (16 bit) cannot 'JUMP' into the far ROM,
#        using this section would cause code relocation failure.
LDFLAGS_DMS	+= -T./ldscripts/msp430f5438-s.ld
LDFLAGS_DML	+= -T./ldscripts/msp430f5438-l.ld

# Release flags (Use dead code elimination flags,
# @see: http://gcc.gnu.org/ml/gcc-help/2003-08/msg00128.html)
CFLAGS_REL	+= -Os -fdata-sections -ffunction-sections -fomit-frame-pointer
LDFLAGS_REL	+= -Wl,--gc-sections -Wl,-s

# Debug flags
CFLAGS_DBG	+= -O1 -ggdb -Wa,--gstabs
LDFLAGS_DBG	+= -Wl,--gc-sections


# ****************************************************************************************
# Build tools

CC		= msp430-elf-gcc
LD		= msp430-elf-ld
AS		= msp430-elf-as
AR		= msp430-elf-ar
OBJCOPY	= msp430-elf-objcopy
SIZE	= msp430-elf-size
GDB		= msp430-elf-gdb
