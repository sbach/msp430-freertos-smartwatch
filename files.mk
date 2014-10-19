# ****************************************************************************************
# Source files list and includes

SOURCE_PATH		= src
FREERTOS_PATH	= freertos
PORT_PATH		= $(FREERTOS_PATH)/portable/MSP430X

INCLUDES += -I. \
	-I $(FREERTOS_PATH)/include \
	-I $(PORT_PATH) \
	-I ./include

SRCS = $(SOURCE_PATH)/main.c \
	$(SOURCE_PATH)/hal/misc.c \
	$(SOURCE_PATH)/hal/ti/ucs.c \
	$(SOURCE_PATH)/hal/ti/pmm.c \
	$(PORT_PATH)/port.c \
	$(PORT_PATH)/portext.S \
	$(FREERTOS_PATH)/tasks.c \
	$(FREERTOS_PATH)/timers.c \
	$(FREERTOS_PATH)/list.c \
	$(FREERTOS_PATH)/queue.c \
	$(FREERTOS_PATH)/portable/MemMang/heap_2.c
