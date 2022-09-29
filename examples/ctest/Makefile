BINARYNAME = main
BUILDDIR = build

OPTFLAG = -O0

EXTLIBDIR = ../../third-party
SHAREDDIR = ../../shared

SOURCES = startup.s \
		  main.cc \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \
		  $(SHAREDDIR)/system/system_ca7.c \
		  $(SHAREDDIR)/system/mmu_ca7.c \

INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR)\

include $(SHAREDDIR)/makefile-common.mk

