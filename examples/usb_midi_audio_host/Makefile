BINARYNAME = main
BUILDDIR = build

OPTFLAG = -O3

EXTLIBDIR = ../../third-party
SHAREDDIR = ../../shared
STM32HALDIR = $(EXTLIBDIR)/STM32MP1xx_HAL_Driver
PATCHEDSTM32HALDIR = $(SHAREDDIR)/stm32hal_patched
USBLIBDIR = $(EXTLIBDIR)/STM32_USB_Host_Library

SOURCES = startup.s \
		  main.cc \
		  syscalls.c \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/system/system_ca7.c \
		  $(SHAREDDIR)/system/mmu_ca7.c \
		  $(SHAREDDIR)/system/irq_init.c \
		  $(SHAREDDIR)/drivers/irq_handler.cc \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_hal.c \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_hal_hcd.c \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_ll_usb.c \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_ll_usb_phy.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_gpio.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_ll_rcc.c \
		  $(USBLIBDIR)/Core/Src/usbh_core.c \
		  $(USBLIBDIR)/Core/Src/usbh_ctlreq.c \
		  $(USBLIBDIR)/Core/Src/usbh_ioreq.c \
		  $(USBLIBDIR)/Core/Src/usbh_pipes.c \
		  usbh_conf.cc \
		  usbh_midi.cc \


INCLUDES = -I. \
		   -I$(PATCHEDSTM32HALDIR) \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR) \
		   -I$(USBLIBDIR)/Core/Inc \

EXTRA_ARCH_CFLAGS = -DUSE_HAL_DRIVER

# LINK_STDLIB = --specs=nosys.specs
LINK_STDLIB = 

include $(SHAREDDIR)/makefile-common.mk

