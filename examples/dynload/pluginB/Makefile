BINARYNAME := pluginB
BUILDDIR := build

OPTFLAG = -O0

SHAREDDIR := ../../../shared
SCRIPTDIR := ../../../scripts

SOURCES = init.cc plugin.cc calc.cc

INCLUDES := -I. 

LFLAGS = $(MCU)  \
	     -Wl,--gc-sections \
		 -Wl,-Map,$(BUILDDIR)/$(BINARYNAME).map,--cref \
		 -nostdlib -nostartfiles -ffreestanding

%.diss : %.so
	arm-none-eabi-objdump -CDz --source $^ > $@

%.nm : %.so
	arm-none-eabi-nm -CA $^ > $@

%.readelf : %.so
	arm-none-eabi-readelf --demangle=auto -a -W $^ > $@


CFLAGS ?= -fno-common \
		 $(ARCH_CFLAGS) \
		 $(MCU) \
		 $(INCLUDES) \
		 -fdata-sections -ffunction-sections \
		 -nostdlib \
		 -nostartfiles \
		 -ffreestanding \
		 $(EXTRACFLAGS)\
		 -c \
		 -fPIC

.DEFAULT: plugin

plugin: $(BUILDDIR)/$(BINARYNAME).so
	arm-none-eabi-objdump -CDz --source $^ > $^.diss
	arm-none-eabi-nm -CA $^ > $^.nm
	arm-none-eabi-readelf --demangle=auto -a -W $^ > $^.readelf

include $(SHAREDDIR)/makefile-common.mk

$(BUILDDIR)/$(BINARYNAME).so: $(OBJECTS)
	@echo "Linking shared library"
	$(LD) -shared $(LFLAGS) -o $@ $(OBJECTS)

