# ****************************************************************************************
# Includes

include common.mk
include files.mk


# ****************************************************************************************
# Variables

OUTDIR ?= build
MSPDEBUG_DRIVER ?= olimex


# ****************************************************************************************
# Utils

COLS =`tput cols`
PAD = $(shell expr $(COLS) - 6)

# Colors
RED=`tput setaf 1`
GREEN=`tput setaf 2`
YELLOW=`tput setaf 3`
GREY=`tput setaf 7`
BLUE=`tput setaf 6`
VIOLET=`tput setaf 5`
NORMAL=`tput sgr0`
BOLD=`tput bold`


# ****************************************************************************************
# Macros

LOG_OUT = tmp.log || touch tmp.err

CC_CHECK = @if test -e tmp.err; \
	then printf "$(BOLD)$(RED)[ERR]$(NORMAL)\n"; \
	elif test -s tmp.log; \
		then printf "$(BOLD)$(YELLOW)[WARN]$(NORMAL)\n"; \
	else printf "$(BOLD)$(GREEN)[OK]$(NORMAL)\n"; fi; \
	cat tmp.log >> build.log; \
	rm -f tmp.err tmp.log;

CMD_CHECK = @if test -e tmp.err; \
	then printf "$(BOLD)$(RED)[ERR]$(NORMAL)\n"; \
	else printf "$(BOLD)$(GREEN)[OK]$(NORMAL)\n"; fi; \
	cat tmp.log >> build.log; \
	rm -f tmp.err tmp.log;


# ****************************************************************************************
# Targets

.PHONY: all
.PHONY: clear
.PHONY: clean
.PHONY: config


# ****************************************************************************************
# Objects to build, and dependency files

OBJS =	$(patsubst %.c, $(OUTDIR)/%.o, $(filter %.c, $(SRCS)))
OBJS +=	$(patsubst %.S, $(OUTDIR)/%.o, $(filter %.S, $(SRCS)))

DEPS := $(patsubst %.o, $(OUTDIR)/%.d, $(filter %.o, $(OBJS)))


# ****************************************************************************************
# Append specific CFLAGS/LDFLAGS

ifneq ($(wildcard include/config.h), )

	DEBUG := $(shell grep "^\s*\#define CONFIG_DEBUG" include/config.h)
	MODEL_SMALL := $(shell grep "^\s*\#define CONFIG_DATA_MODEL_SMALL" include/config.h)
	MODEL_LARGE := $(shell grep "^\s*\#define CONFIG_DATA_MODEL_LARGE" include/config.h)

	ifeq ($(DEBUG),)
		TARGET	:= RELEASE
		CFLAGS	+= $(CFLAGS_REL)
		LDFLAGS	+= $(LDFLAGS_REL)
	else
		TARGET	:= DEBUG
		CFLAGS	+= $(CFLAGS_DBG)
		LDFLAGS	+= $(LDFLAGS_DBG)
	endif

	ifeq ($(MODEL_SMALL),)
		CFLAGS	+= $(CFLAGS_DML)
	else ifeq ($(MODEL_LARGE),)
		CFLAGS	+= $(CFLAGS_DMS)
	endif

endif


# ****************************************************************************************
# Main rule

all: required $(OUTDIR)/firmware.elf

# ****************************************************************************************
# Required files rules

required: include/config.h

include/config.h:
	@printf "$(GREY)Please do a 'make config' first!$(NORMAL)\n" && false

# ****************************************************************************************
# Basic rules

$(OUTDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@printf "$(GREY)%-${PAD}s" "(CC) $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $< 2>> $(LOG_OUT)
	$(CC_CHECK)

$(OUTDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@printf "$(GREY)%-${PAD}s" "(CC) $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $< 2>> $(LOG_OUT)
	$(CC_CHECK)

# ****************************************************************************************
# Include dependencies rules

-include $(DEPS)

# ****************************************************************************************
# Specific rules

$(OUTDIR)/firmware.elf: $(OBJS)
	@printf "$(BOLD)Firmware generation: ELF$(NORMAL)\n"

	@printf "$(GREY)%-${PAD}s" "(CC) $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $+ 2>> $(LOG_OUT)
	$(CC_CHECK)

	@rm -f output.map

# ****************************************************************************************
# Top rules

config:
	@if test -e include/config.h; then \
		mv include/config.h include/config.h.old; \
	fi
	@printf "$(GREY)Configuring...$(NORMAL)\n"
	@cp include/config.h.in include/config.h

install: $(OUTDIR)/firmware.elf
	@echo "Installing the new firmware via USB..."
	sudo mspdebug $(MSPDEBUG_DRIVER) "prog $(OUTDIR)/firmware.elf"

debug:
	@echo "Starting mspdebug..."
	@sudo mspdebug --force-reset $(MSPDEBUG_DRIVER) gdb &> /dev/null
	@$(GDB) -nx -x .gdbinit -- $(OUTDIR)/firmware.elf

size: $(OUTDIR)/firmware.elf
	@$(SIZE) $(OUTDIR)/firmware.elf

clear: clean
	@rm -Rf ./include/config.h
	@rm -Rf ./include/config.h.old

clean:
	@rm -Rf ./$(OUTDIR)/ build.log
