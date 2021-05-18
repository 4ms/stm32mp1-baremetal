# Makefile by Dan Green <danngreen1@gmail.com>, 2021

$(info --------------------)
$(info Building for MP1 A7 core)

BUILDDIR = $(BUILDDIR_A7)

LOADFILE = linkscript_ca7.ld
STARTUP	= startup_ca7.s

OPTFLAG = -O0

SOURCES = $(STARTUP) \
		  main_ca7.c \
		  $(LIBDIR)/system/irq_ctrl.c \
		  $(LIBDIR)/system/libc_stub.c \
		  $(LIBDIR)/system/libcpp_stub.cc \
		  $(LIBDIR)/system/system_ca7.c \
		  $(LIBDIR)/system/mmu_ca7.c \
		  $(LIBDIR)/mdrivlib/interrupt.cc \

INCLUDES = -I. \
		   -I$(LIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(LIBDIR)/CMSIS/Core_A/Include \
		   -I$(LIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(LIBDIR) \

ARCH_CFLAGS += -DUSE_FULL_LL_DRIVER \
			  -DSTM32MP157Cxx \
			  -DSTM32MP1 \
			  -DCORE_CA7 \

MCU =  -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mlittle-endian -mfloat-abi=hard -mthumb-interwork

#### U-BOOT
UIMG  		= $(BUILDDIR)/a7-main.uimg
LOADADDR 	= 0xC2000040
ENTRYPOINT 	= $(LOADADDR)
UBOOTDIR 	= ../u-boot
UBOOTSRCDIR = $(UBOOTDIR)/u-boot-stm32mp1-baremetal
UBOOTBUILDDIR = $(UBOOTDIR)/build
UBOOT_MKIMAGE = $(UBOOTBUILDDIR)/tools/mkimage
#####

image: $(UIMG)

$(UBOOT_MKIMAGE): $(UBOOTSRCDIR)
	cd $(UBOOTSRCDIR) && make O=$(PWD)/$(UBOOTBUILDDIR) CROSS_COMPILE=arm-none-eabi- stm32mp15x_baremetal_defconfig
	cd $(UBOOTSRCDIR) && make -j16 O=$(PWD)/$(UBOOTBUILDDIR) CROSS_COMPILE=arm-none-eabi- all

$(UIMG): $(BUILDDIR)/main.bin $(UBOOT_MKIMAGE)
	@$(UBOOT_MKIMAGE) -A arm -C none -T kernel -a $(LOADADDR) -e $(ENTRYPOINT) -d $(BIN) $@

clean_uboot:
	rm -rf $(UBOOTBUILDDIR)


include makefile_common.mk

