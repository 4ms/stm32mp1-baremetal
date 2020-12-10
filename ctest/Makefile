STARTUP = startup_stm32mp157caax.s
LINKSCR = stm32mp15xx_m4.ld
BUILDDIR = build

PHONY: all

all: $(BUILDDIR)/app.bin

$(BUILDDIR)/startup.o: $(STARTUP)
	@mkdir -p $(BUILDDIR)
	arm-none-eabi-as -o $(BUILDDIR)/startup.o $(STARTUP)

$(BUILDDIR)/app.o: app.c
	@mkdir -p $(BUILDDIR)
	arm-none-eabi-gcc -c -nostdlib -nostartfiles -o $(BUILDDIR)/app.o app.c

$(BUILDDIR)/app.bin: $(BUILDDIR)/app.o $(BUILDDIR)/startup.o $(LINKSCR)
	@mkdir -p $(BUILDDIR)
	arm-none-eabi-ld -T $(LINKSCR) -o $(BUILDDIR)/app.elf $(BUILDDIR)/startup.o $(BUILDDIR)/app.o
	arm-none-eabi-objcopy -O binary $(BUILDDIR)/app.elf $(BUILDDIR)/app.bin


# make -C ../common_uboot/ vexpress_ca9x4_config ARCH=arm CROSS_COMPILE=arm-none-eabi-
# make -C ../common_uboot/ all ARCH=arm CROSS_COMPILE=arm-none-eabi-

# ../u-boot-stm32mp-2020.01-r0/u-boot-stm32mp1-baremetal/tools/mkimage -A arm -C none -T kernel -a 0xC0000040 -e 0xC0000040 -d cenv.bin bare-arm.uimg
#./create-sd.sh sdcard.img bare-arm.uimg
