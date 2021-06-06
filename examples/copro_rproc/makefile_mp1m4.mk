# Makefile by Dan Green <danngreen1@gmail.com>, public domain

$(info --------------------)
$(info Building for MP1 M4 core)

BUILDDIR = $(BUILDDIR_M4)

LOADFILE = linkscript_cm4.ld
STARTUP = startup_cm4.ld

OPTFLAG = -O0

SOURCES  = $(STARTUP)
SOURCES  += main_cm4.cc \
			system_cm4.c \
			$(LIBDIR)/system/libc_stub.c \
			$(LIBDIR)/system/libcpp_stub.cc

INCLUDES = -I. \
		   -I$(LIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(LIBDIR)/CMSIS/Core/Include \
		   -I$(LIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(LIBDIR) \

MCU = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mlittle-endian -mfloat-abi=hard

ARCH_CFLAGS = -DUSE_FULL_LL_DRIVER \
			  -DSTM32MP157Cxx \
			  -DSTM32MP1 \
			  -DCORE_CM4 \
			  -DARM_MATH_CM4 \

include makefile_common.mk

