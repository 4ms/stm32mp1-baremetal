BINARYNAME = main
BUILDDIR = build

OPTFLAG = -O1

EXTLIBDIR = ../../third-party
SHAREDDIR = ../shared
STM32HALDIR = $(EXTLIBDIR)/STM32MP1xx_HAL_Driver

SOURCES = main.cc \
		  mmu_ca7.c \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/system/new.cc \
		  mdrivlib/drivers/i2c.cc \
		  mdrivlib/drivers/pin.cc \
		  mdrivlib/drivers/codec_WM8731.cc \
		  mdrivlib/drivers/sai.cc \
		  mdrivlib/drivers/timekeeper.cc \
		  mdrivlib/drivers/tim.cc \
		  mdrivlib/target/stm32mp1_ca7/drivers/cycle_counter.cc \
		  mdrivlib/target/stm32mp1_ca7/drivers/hal_handlers.cc \
		  mdrivlib/target/stm32mp1_ca7/drivers/interrupt_handler.cc \
		  mdrivlib/target/stm32mp1_ca7/boot/startup_ca7.s \
		  mdrivlib/target/stm32mp1_ca7/boot/system_ca7.c \
		  mdrivlib/target/stm32mp1_ca7/boot/irq_ctrl.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_cortex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_dma.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_dma_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_i2c.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_i2c_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_mdma.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_sai.c \
		  #$(STM32HALDIR)/Src/stm32mp1xx_ll_gpio.c \

INCLUDES = -I. \
		   -I$(STM32HALDIR)/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR)/ \
		   -Iutil \
		   -Imdrivlib \
		   -Imdrivlib/drivers \
		   -Imdrivlib/target/stm32mp1 \
		   -Imdrivlib/target/stm32mp1_ca7 \
		   -Imdrivlib/target/stm32mp1/drivers \
		   -Imdrivlib/target/stm32mp1_ca7/boot \
		   -Imdrivlib/target/stm32mp1_ca7/drivers \

EXTRACFLAGS = -DUSE_HAL_DRIVER \
			  -DUSE_FULL_LL_DRIVER \
			  -DSTM32MP157Cxx \
			  -DSTM32MP1 \
			  -DCORE_CA7 \
			  
EXTRACXXFLAGS = -Wno-volatile


include ../shared/makefile-common.mk

