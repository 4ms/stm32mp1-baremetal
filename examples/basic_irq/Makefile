BINARYNAME = main
BUILDDIR = build

STARTUP = startup.s
LINKSCR = linkscript.ld

EXTLIBDIR = ../../third-party
UBOOTDIR = $(EXTLIBDIR)/u-boot/build
LIBDIR = ../shared

SOURCES = $(STARTUP) \
		  main.c \
		  $(LIBDIR)/irq_init.c \
		  $(LIBDIR)/libc_stub.c \
		  $(LIBDIR)/libcpp_stub.cc \
		  $(LIBDIR)/system_ca7.c \
		  $(LIBDIR)/mmu_ca7.c \

INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(LIBDIR)\

include ../shared/makefile-common.mk

