$(info --------------------)
$(info Building for MP1 M4 core)

BUILDDIR = $(BUILDDIR_M4)

LINKSCR = linkscript_cm4.ld

OPTFLAG = -O0

SOURCES  += startup_cm4.s \
			main_cm4.cc \
			system_cm4.c \
			$(SHAREDDIR)/system/libc_stub.c \
			$(SHAREDDIR)/system/libcpp_stub.cc

INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR) \

MCU = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mlittle-endian -mfloat-abi=hard

ARCH_CFLAGS = -DUSE_FULL_LL_DRIVER \
			  -DSTM32MP157Cxx \
			  -DSTM32MP1 \
			  -DCORE_CM4 \
			  -DARM_MATH_CM4 \

include $(SHAREDDIR)/makefile-common.mk

