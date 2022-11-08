BINARYNAME = main
BUILDDIR = build

OPTFLAG = -O0

EXTLIBDIR = ../../third-party
SHAREDDIR = ../shared
STM32HALDIR = $(EXTLIBDIR)/STM32MP1xx_HAL_Driver
LVGL_DIR_NAME ?= lvgl#The name of the lvgl folder (change this if you have renamed it)
LVGL_DIR ?= ${shell pwd}#The path where the lvgl folder is
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/lvgl.mk

SOURCES = startup.s \
		  main.cc \
		  $(SHAREDDIR)/system/irq_init.c \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/system/system_ca7.c \
		  mmu_ca7.c \
		  syscalls.c \
		  lcd.c \
		  ft6x06.c \
		  otm8009a.c \
		  stm32mp1xx_hal_ltdc.c \
		  stm32mp1xx_hal_ltdc_ex.c \
		  stm32mp1xx_hal_dsi.c \
		  $(SHAREDDIR)/stm32hal_patched/stm32mp1xx_hal.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_dma.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_dma_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_gpio.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_i2c.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_i2c_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_mdma.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_tim.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_tim_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_uart.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_uart_ex.c \
		  $(CSRCS) \

INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR)\

LINK_STDLIB =

include makefile-common.mk

