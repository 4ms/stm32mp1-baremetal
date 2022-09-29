BINARYNAME = fsbl
BUILDDIR = build

OPTFLAG = -O3

EXTLIBDIR = ../../third-party
SHAREDDIR = ../../shared

SOURCES = startup.s \
		  main.cc \
		  systeminit.c \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/print.cc \
		  $(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Src/stm32mp1xx_ll_usart.c \
		  $(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Src/stm32mp1xx_ll_rcc.c \
		  $(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Src/stm32mp1xx_hal.c \
		  $(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Src/stm32mp1xx_ll_sdmmc.c \
		  $(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Src/stm32mp1xx_hal_sd.c \
		  ddr/stm32mp1_ddr.cc \
		  ddr/stm32mp1_ram.cc \
		  ddr/ram_tests.cc \
		  uboot-port/common/memsize.c \
		  uboot-port/lib/crc32.c \
		  norflash/qspi_ll.c \
		  norflash/qspi_norflash_read.c \
		  gpt/gpt.cc \

INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR) \
		   -Iddr/ \
		   -Iuboot-port/include \
		   -Iuboot-port/arch/arm/include \

all: image

include $(SHAREDDIR)/makefile-common.mk

SD_DISK_DEV ?= /dev/disk4

image: $(BIN)
	python3 fsbl_header.py $(BUILDDIR)/$(BINARYNAME).bin $(BUILDDIR)/$(BINARYNAME).stm32
	@ls -l $(BUILDDIR)/$(BINARYNAME).stm32

load: image
	@read -p "What is the disk device (Enter for $(SD_DISK_DEV)): " DISK && \
	DISK=$${DISK:-$(SD_DISK_DEV)} && \
	echo "Writing to $${DISK}s1 and $${DISK}s2" && \
	sudo dd if=$(BUILDDIR)/$(BINARYNAME).stm32 of=$${DISK}s1 && \
	sudo dd if=$(BUILDDIR)/$(BINARYNAME).stm32 of=$${DISK}s2 && \
	diskutil unmountDisk $${DISK}

