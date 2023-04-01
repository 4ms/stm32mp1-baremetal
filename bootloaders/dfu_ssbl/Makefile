BINARYNAME = main
BUILDDIR = build

OPTFLAG = -O0

EXTLIBDIR = ../../third-party
SHAREDDIR = ../../shared
STM32HALDIR = $(EXTLIBDIR)/STM32MP1xx_HAL_Driver
PATCHEDSTM32HALDIR = $(SHAREDDIR)/stm32hal_patched
USBLIBDIR = $(EXTLIBDIR)/STM32_USB_Device_Library

SOURCES = startup.s \
		  main.cc \
		  print.cc \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/system/system_ca7.c \
		  $(SHAREDDIR)/system/mmu_ca7.c \
		  $(SHAREDDIR)/system/irq_init.c \
		  $(SHAREDDIR)/drivers/irq_handler.cc \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_hal.c \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_hal_pcd.c \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_hal_pcd_ex.c \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_ll_usb.c \
		  $(PATCHEDSTM32HALDIR)/stm32mp1xx_ll_usb_phy.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_gpio.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_qspi.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_ll_rcc.c \
		  usb_dfu/usbd_conf.c \
		  usb_dfu/usbd_desc.c \
		  usb_dfu/usbd_dfu_media.cc \
		  usb_dfu/norflash/qspi_flash_driver.cc \
		  app_loader/norflash/qspi_ll.c \
		  app_loader/norflash/qspi_norflash_read.c \
		  $(USBLIBDIR)/Class/DFU/Src/usbd_dfu.c \
		  $(USBLIBDIR)/Core/Src/usbd_core.c \
		  $(USBLIBDIR)/Core/Src/usbd_ctlreq.c \
		  $(USBLIBDIR)/Core/Src/usbd_ioreq.c \


INCLUDES = -I. \
		   -Iusb_dfu \
		   -I$(PATCHEDSTM32HALDIR) \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR) \
		   -I$(USBLIBDIR)/Class/DFU/Inc \
		   -I$(USBLIBDIR)/Core/Inc \

EXTRA_ARCH_CFLAGS = -DUSE_HAL_DRIVER -DBOARD_CONF_PATH=board_conf/brainboard_p3_conf.hh

LINK_STDLIB = --specs=nosys.specs
# LINK_STDLIB =

include $(SHAREDDIR)/makefile-common.mk

