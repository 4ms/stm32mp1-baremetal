SDCARD_MOUNT_PATH ?= /Volumes/BAREAPP

LINKSCR ?= linkscript.ld
EXTLIBDIR ?= ../../third-party
UBOOTDIR ?= $(EXTLIBDIR)/u-boot/build
BUILDDIR ?= build
BINARYNAME ?= main
UIMAGENAME ?= $(BUILDDIR)/a7-main.uimg
SCRIPTDIR ?= ../../scripts

OBJDIR = $(BUILDDIR)/obj/obj
LOADADDR 	= 0xC2000040
ENTRYPOINT 	= 0xC2000040

OBJECTS   = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(SOURCES))))
DEPS   	  = $(addprefix $(OBJDIR)/, $(addsuffix .d, $(basename $(SOURCES))))

MCU ?=  -mcpu=cortex-a7 -march=armv7ve -mfpu=neon-vfpv4 -mlittle-endian -mfloat-abi=hard

EXTRA_ARCH_CFLAGS ?= 

ARCH_CFLAGS ?= -DUSE_FULL_LL_DRIVER \
			   -DSTM32MP157Cxx \
			   -DSTM32MP1 \
			   -DCORE_CA7 \
			   $(EXTRA_ARCH_CFLAGS) \

OPTFLAG ?= -O0

AFLAGS = $(MCU)

CFLAGS = -g2 \
		 -fno-common \
		 $(ARCH_CFLAGS) \
		 $(MCU) \
		 $(INCLUDES) \
		 -fdata-sections -ffunction-sections \
		 -nostartfiles \
		 -ffreestanding \
		 $(EXTRACFLAGS)\

CXXFLAGS = $(CFLAGS) \
		-std=c++2a \
		-fno-rtti \
		-fno-exceptions \
		-fno-unwind-tables \
		-ffreestanding \
		-fno-threadsafe-statics \
		-mno-unaligned-access \
		-Werror=return-type \
		-Wdouble-promotion \
		-Wno-register \
		-Wno-volatile \
		 $(EXTRACXXFLAGS) \

LINK_STDLIB ?= -nostdlib

LFLAGS = -Wl,--gc-sections \
		 -Wl,-Map,$(BUILDDIR)/$(BINARYNAME).map,--cref \
		 $(MCU)  \
		 -T $(LINKSCR) \
		 $(LINK_STDLIB) \
		 -nostartfiles \
		 -ffreestanding \
		 $(EXTRALDFLAGS) \

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

elf: $(ELF)

install:
	cp $(UIMAGENAME) $(SDCARD_MOUNT_PATH)
	diskutil unmount $(SDCARD_MOUNT_PATH)

install-mp1-boot:
	@if [ "$${SD_DISK_DEVPART}" = "" ]; then echo "Please specify the disk and partition like this: make install-mp1-boot SD_DISK_DEVPART=/dev/diskXs3"; \
	else \
	echo "sudo dd if=${UIMAGENAME} of=$${SD_DISK_DEVPART}" && \
	sudo dd if=${UIMAGENAME} of=$${SD_DISK_DEVPART};  fi

$(OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(AS) $(AFLAGS) $< -o $@ 

$(OBJDIR)/%.o: %.c $(OBJDIR)/%.d
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(CC) -c $(DEPFLAGS) $(OPTFLAG) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: %.c[cp]* $(OBJDIR)/%.d
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(CXX) -c $(DEPFLAGS) $(OPTFLAG) $(CXXFLAGS) $< -o $@

$(ELF): $(OBJECTS) $(LINKSCR)
	$(info Linking...)
	@$(LD) $(LFLAGS) -o $@ $(OBJECTS) 

$(BIN): $(ELF)
	$(OBJCPY) -O binary $< $@

$(HEX): $(ELF)
	@$(OBJCPY) --output-target=ihex $< $@
	@$(SZ) $(SZOPTS) $(ELF)

$(UIMAGENAME): $(BIN)
	$(info Creating uimg file)
	python $(SCRIPTDIR)/uimg_header.py $< $@

%.d: ;

clean:
	rm -rf build

ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif

.PRECIOUS: $(DEPS) $(OBJECTS) $(ELF)
.PHONY: all clean install install-mp1-boot

.PHONY: compile_commands
compile_commands:
	compiledb make
	compdb -p ./ list > compile_commands.tmp 2>/dev/null
	rm compile_commands.json
	mv compile_commands.tmp compile_commands.json
