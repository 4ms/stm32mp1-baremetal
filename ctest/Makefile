STARTUP = startup.s
LINKSCR = linkscript.ld

UBOOTDIR =../u-boot-stm32mp-2020.01-r0/build_baremetal
BUILDDIR = build

INCLUDES = -I. \
		   -IDrivers/STM32MP1xx_HAL_Driver/Inc \
		   -IDrivers/CMSIS/Core_A/Include \
		   -IDrivers/CMSIS/Device/ST/STM32MP1xx/Include \

CFLAGS = -nostdlib \
		 -nostartfiles \

DEFS = -DCORE_CA7 \
	   -DSTM32MP157Cxx \

PHONY: all

#Todo: need a .uimg target. Seems like mkimage converts elf to uimg
all: $(BUILDDIR)/app.bin
	$(UBOOTDIR)/tools/mkimage -A arm -C none -T kernel -a 0xC2000040 -e 0xC2000040 -d build/app.elf bare-arm.uimg

$(BUILDDIR)/startup.o: $(STARTUP)
	@mkdir -p $(BUILDDIR)
	arm-none-eabi-as -o $(BUILDDIR)/startup.o $(STARTUP)

$(BUILDDIR)/app.o: app.c
	@mkdir -p $(BUILDDIR)
	arm-none-eabi-gcc -c $(CFLAGS) $(DEFS) $(INCLUDES) -o $(BUILDDIR)/app.o app.c

#Todo: this should be .elf, then objcopy creates the .bin target
$(BUILDDIR)/app.bin: $(BUILDDIR)/app.o $(BUILDDIR)/startup.o $(LINKSCR)
	@mkdir -p $(BUILDDIR)
	arm-none-eabi-ld -T $(LINKSCR) -o $(BUILDDIR)/app.elf $(BUILDDIR)/startup.o $(BUILDDIR)/app.o
	arm-none-eabi-objcopy -O binary $(BUILDDIR)/app.elf $(BUILDDIR)/app.bin


#./create-sd.sh sdcard.img bare-arm.uimg
