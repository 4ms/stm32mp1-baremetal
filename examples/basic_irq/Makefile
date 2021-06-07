BINARYNAME = main
BUILDDIR = build

STARTUP = startup.s
LINKSCR = linkscript.ld

EXTLIBDIR = ../../third-party
UBOOTDIR = $(EXTLIBDIR)/u-boot/build
SHAREDDIR = ../shared

SOURCES = $(STARTUP) \
		  main.c \
		  $(SHAREDDIR)/irq_init.c \
		  $(SHAREDDIR)/libc_stub.c \
		  $(SHAREDDIR)/libcpp_stub.cc \
		  $(SHAREDDIR)/system_ca7.c \
		  $(SHAREDDIR)/mmu_ca7.c \

INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR)\

include ../shared/makefile-common.mk

