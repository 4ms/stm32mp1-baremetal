BINARYNAME = fsbl
BUILDDIR = build

OPTFLAG = -O0

EXTLIBDIR = ../../../third-party
SHAREDDIR = ../

SOURCES = startup.s \
		  main.cc \
		  systeminit.c \
		  $(SHAREDDIR)/system/libc_stub.c \
		  $(SHAREDDIR)/system/libcpp_stub.cc \

INCLUDES = -I. \
		   -I$(EXTLIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(EXTLIBDIR)/CMSIS/Core_A/Include \
		   -I$(EXTLIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(SHAREDDIR)\

image: 
	python3 fix_header.py $(BUILDDIR)/$(BINARYNAME).bin $(BUILDDIR)/$(BINARYNAME).stm32

load: all
	python3 fix_header.py $(BUILDDIR)/$(BINARYNAME).bin $(BUILDDIR)/$(BINARYNAME).stm32
	sudo dd if=$(BUILDDIR)/$(BINARYNAME).stm32 of=/dev/disk4s1
	sudo dd if=$(BUILDDIR)/$(BINARYNAME).stm32 of=/dev/disk4s2
	diskutil unmountDisk /dev/disk4

include ../makefile-common.mk
