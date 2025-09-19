BINARYNAME = main
BUILDDIR = build

OPTFLAG = -O3

EXTLIBDIR = ../../third-party
SHAREDDIR = ../../shared
STM32HALDIR = $(EXTLIBDIR)/STM32MP1xx_HAL_Driver
mdrivlibdir = lib/mdrivlib

SOURCES = main.cc \
		  mmu_ca7.c \
		  startup.s \
		  get_tick.cc \
		  lib/util/math_tables.cc \
		  mdrivlib/drivers/i2c.cc \
		  mdrivlib/drivers/pin.cc \
		  mdrivlib/drivers/codec_PCM3060.cc \
		  mdrivlib/target/stm32mp1_ca7/drivers/sai_tdm.cc \
		  mdrivlib/target/stm32mp1_ca7/drivers/cycle_counter.cc \
		  mdrivlib/target/stm32mp1_ca7/drivers/interrupt_handler.cc \
		  mdrivlib/target/stm32mp1_ca7/boot/irq_ctrl.c \
		  $(SHAREDDIR)/system/system_ca7.c \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/system/new.cc \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_cortex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_dma.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_dma_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_i2c.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_i2c_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_sai.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc_ex.c \
		  $(EXTLIBDIR)/DaisySP/Source/Control/adenv.cpp \
		  $(EXTLIBDIR)/DaisySP/Source/Effects/reverbsc.cpp \
		  $(EXTLIBDIR)/DaisySP/Source/Synthesis/oscillator.cpp \
		  $(EXTLIBDIR)/DaisySP/Source/Utility/metro.cpp \


INCLUDES = -I. \
		   -I.. \
		   -Ilib \
		   -Icpputil \
		   -Imdrivlib/ \
		   -Imdrivlib/target/stm32mp1 \
		   -Imdrivlib/target/stm32mp1_ca7 \
		   -I$(SHAREDDIR) \
		   -I$(SHAREDDIR)/drivers \
		   -I$(STM32HALDIR)/Inc \
		   -I$(EXTLIBDIR)/gcem/include \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(EXTLIBDIR)/DaisySP/Source \
		   -I$(EXTLIBDIR)/DaisySP/Source/Utility \

EXTRACFLAGS = -DUSE_HAL_DRIVER \
			  -DUSE_FULL_LL_DRIVER \
			  -DSTM32MP157Cxx \
			  -DSTM32MP1 \
			  -DCORE_CA7 \
				-mfpu=neon-vfpv4 \
				-mfloat-abi=hard \
				-mthumb-interwork \
				-mtune=cortex-a7 \
				-mno-unaligned-access \
				-funsafe-math-optimizations \
				-mvectorize-with-neon-quad \
				-Wfatal-errors
			  
EXTRACXXFLAGS =  \


include $(SHAREDDIR)/makefile-common.mk

