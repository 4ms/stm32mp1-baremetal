# Makefile by Dan Green <danngreen1@gmail.com>, 2021

$(info --------------------)
$(info Building for MP1 A7 core)

BUILDDIR = $(BUILDDIR_A7)

LINKSCR = linkscript_ca7.ld

OPTFLAG = -O0

SOURCES = startup_ca7.s \
		  main_ca7.c \
		  $(SHAREDDIR)/system/irq_init.c \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/system/system_ca7.c \
		  $(SHAREDDIR)/system/mmu_ca7.c \

INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR) \

include $(SHAREDDIR)/makefile-common.mk
