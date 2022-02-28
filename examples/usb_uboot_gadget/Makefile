BINARYNAME = main
BUILDDIR = build

OPTFLAG = -O0

EXTLIBDIR = ../../third-party
SHAREDDIR = ../shared
STM32HALDIR = $(EXTLIBDIR)/STM32MP1xx_HAL_Driver
PATCHEDSTM32HALDIR = $(SHAREDDIR)/stm32hal_patched

# USBLIBDIR = $(EXTLIBDIR)/STM32_USB_Device_Library

SOURCES = startup.s \
		  main.cc \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/system/system_ca7.c \
		  $(SHAREDDIR)/system/mmu_ca7.c \
		  $(SHAREDDIR)/system/irq_init.c \
		  $(SHAREDDIR)/drivers/irq_handler.cc \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_hal.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_dma.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_dma_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_gpio.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_ll_rcc.c \
		  $(SHAREDDIR)/printf/printf.c \
		  ubg/cmd/usb_mass_storage.c \
		  ubg/fake_blk_dev.c \
		  ubg/fake_uclass_dm.c \
		  ubg/drivers/usb/gadget/udc/udc-core.c \
		  ubg/drivers/usb/gadget/udc/udc-uclass.c \
		  ubg/drivers/usb/gadget/dwc2_udc_otg.c \
		  #ubg/board/st/stm32mp1/stm32mp1.c \
		  #ubg/board/st/stm32mp1/board.c \
		  #ubg/drivers/usb/gadget/f_mass_storage.c \


INCLUDES = -I. \
		   -I$(PATCHEDSTM32HALDIR) \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR) \
		   -Iubg/include \
		   -Iubg/arch/arm/include \

EXTRA_ARCH_CFLAGS = -DUSE_HAL_DRIVER
EXTRA_ARCH_CFLAGS += -D__KERNEL__
EXTRA_ARCH_CFLAGS += -DSTM32MP1BAREMETAL

# LINK_STDLIB =
# LINK_STDLIB = --specs=nano.specs #doesn't define sbrk, _write, _isatty etc
LINK_STDLIB = --specs=nosys.specs

include ../shared/makefile-common.mk

