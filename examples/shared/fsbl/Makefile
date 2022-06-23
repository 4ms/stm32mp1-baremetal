BINARYNAME = fsbl
BUILDDIR = build

OPTFLAG = -O0

EXTLIBDIR = ../../../third-party
SHAREDDIR = ../

SOURCES = startup.s \
		  main.cc \
		  systeminit.c \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Src/stm32mp1xx_ll_usart.c \
		  $(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Src/stm32mp1xx_ll_rcc.c \
		  ddr/stm32mp1_ddr.c \


INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR) \
		   -Iddr/ \
		   -Iddr/uboot-port/include \
		   -Iddr/uboot-port/arch/arm/include \

include ../makefile-common.mk

image: 
	python3 fix_header.py $(BUILDDIR)/$(BINARYNAME).bin $(BUILDDIR)/$(BINARYNAME).stm32

load: all image
	@read -p "What is the disk device (Enter for /dev/disk4): " DISK && \
	DISK=$${DISK:-/dev/disk4} && \
	echo "Writing to $${DISK}s1 and $${DISK}s2" && \
	sudo dd if=$(BUILDDIR)/$(BINARYNAME).stm32 of=$${DISK}s1 && \
	sudo dd if=$(BUILDDIR)/$(BINARYNAME).stm32 of=$${DISK}s2 && \
	diskutil unmountDisk $${DISK}

