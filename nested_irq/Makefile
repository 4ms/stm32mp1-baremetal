BINARYNAME = main
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
UIMAGENAME = $(BUILDDIR)/a7-main.uimg
LOADADDR 	= 0xC2000040
ENTRYPOINT 	= 0xC2000040

STARTUP = startup.s
LINKSCR = linkscript.ld

UBOOTDIR = ../u-boot/build
LIBDIR = ../lib

SOURCES = $(STARTUP) \
		  main.c \
		  new.cc \
		  $(LIBDIR)/mdrivlib/interrupt.cc \
		  $(LIBDIR)/system/irq_ctrl.c \
		  $(LIBDIR)/system/libc_stub.c \
		  $(LIBDIR)/system/libcpp_stub.cc \
		  $(LIBDIR)/system/system_ca7.c \
		  $(LIBDIR)/system/mmu_ca7.c \

OBJECTS   = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(SOURCES))))
DEPS   	  = $(addprefix $(OBJDIR)/, $(addsuffix .d, $(basename $(SOURCES))))

INCLUDES = -I. \
		   -I$(LIBDIR)/STM32MP1xx_HAL_Driver/Inc \
		   -I$(LIBDIR)/CMSIS/Core_A/Include \
		   -I$(LIBDIR)/CMSIS/Device/ST/STM32MP1xx/Include \
		   -I$(LIBDIR)\
		   -I$(LIBDIR)\

MCU =  -mcpu=cortex-a7 -march=armv7-a -mfpu=neon-vfpv4 -mlittle-endian -mfloat-abi=hard

ARCH_CFLAGS += -DUSE_FULL_LL_DRIVER \
			  -DSTM32MP157Cxx \
			  -DSTM32MP1 \
			  -DCORE_CA7 \

OPTFLAG = -O0

AFLAGS = $(MCU)

CFLAGS = -g2 \
		 -fno-common \
		 $(ARCH_CFLAGS) \
		 $(MCU) \
		 $(INCLUDES) \
		 -fdata-sections -ffunction-sections \
		 -nostartfiles \
		 -nostdlib \
		 -ffreestanding

CXXFLAGS = $(CFLAGS) \
		-std=c++2a \
		-fno-rtti \
		-fno-exceptions \
		-fno-unwind-tables \
		-ffreestanding \
		-fno-threadsafe-statics \
		-Werror=return-type \
		-Wdouble-promotion \
		-Wno-register \


LFLAGS = -Wl,--gc-sections \
	-Wl,-Map,$(BUILDDIR)/$(BINARYNAME).map,--cref \
	$(MCU)  \
	-T $(LINKSCR) \
	-nostdlib \
	-nostartfiles \
	-ffreestanding

DEPFLAGS = -MMD -MP -MF $(OBJDIR)/$(basename $<).d

ARCH 	= arm-none-eabi
CC 		= $(ARCH)-gcc
CXX 	= $(ARCH)-g++
LD 		= $(ARCH)-g++
AS 		= $(ARCH)-as
OBJCPY 	= $(ARCH)-objcopy
OBJDMP 	= $(ARCH)-objdump
GDB 	= $(ARCH)-gdb
SZ 		= $(ARCH)-size

SZOPTS 	= -d

ELF 	= $(BUILDDIR)/$(BINARYNAME).elf
HEX 	= $(BUILDDIR)/$(BINARYNAME).hex
BIN 	= $(BUILDDIR)/$(BINARYNAME).bin

all: Makefile $(ELF) $(UIMAGENAME)

install:
	cp $(UIMAGENAME) /Volumes/BAREAPP/
	diskutil unmount /Volumes/BAREAPP
	#../scripts/copy-app-to-sdcard.sh $(UIMAGENAME) /dev/disk3s4

$(OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(AS) $(AFLAGS) $< -o $@ > $(addprefix $(BUILDDIR)/, $(addsuffix .lst, $(basename $<)))

$(OBJDIR)/%.o: %.c $(OBJDIR)/%.d
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(CC) -c $(DEPFLAGS) $(OPTFLAG) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: %.cc $(OBJDIR)/%.d
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(CXX) -c $(DEPFLAGS) $(OPTFLAG) $(CXXFLAGS) $< -o $@

$(OBJDIR)/%.o: %.cpp $(OBJDIR)/%.d
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(CXX) -c $(DEPFLAGS) $(OPTFLAG) $(CXXFLAGS) $< -o $@

$(ELF): $(OBJECTS) $(LINKSCR)
	$(info Linking...)
	@$(LD) $(LFLAGS) -o $@ $(OBJECTS)

$(BIN): $(ELF)
	$(OBJCPY) -O binary $< $@

$(UIMAGENAME): $(BIN)
	$(info Creating uimg file)
	@$(UBOOTDIR)/tools/mkimage -A arm -C none -T kernel -a $(LOADADDR) -e $(ENTRYPOINT) -d $< $@

%.d: ;

clean:
	rm -rf build

ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif

.PRECIOUS: $(DEPS) $(OBJECTS) $(ELF)
.PHONY: all

