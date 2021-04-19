BINARYNAME = app
BUILDDIR = build
UIMAGENAME = $(BUILDDIR)/bare-arm.uimg
LOADADDR 	= 0xC2000040
ENTRYPOINT 	= 0xC2000040

STARTUP = startup.s
LINKSCR = linkscript.ld

UBOOTDIR =../u-boot-stm32mp-2020.01-r0/build_baremetal

SOURCES = $(STARTUP) \
		  app.c

OBJECTS   = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))
DEPS   	  = $(addprefix $(BUILDDIR)/, $(addsuffix .d, $(basename $(SOURCES))))

INCLUDES = -I. \
		   -IDrivers/STM32MP1xx_HAL_Driver/Inc \
		   -IDrivers/CMSIS/Core_A/Include \
		   -IDrivers/CMSIS/Device/ST/STM32MP1xx/Include \

MCU = 

ARCH_CFLAGS = -DCORE_CA7 \
			  -DSTM32MP157Cxx \

OPTFLAG = -O0

AFLAGS = $(MCU)

CFLAGS = -g2 \
		 -nostdlib \
		 -nostartfiles \
		 $(ARCH_CFLAGS) \
		 $(MCU) \
		 $(INCLUDES) \
		 -fno-common \
		 -fdata-sections -ffunction-sections \

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
		-Wno-volatile \


LFLAGS = -T $(LINKSCR) \
	-Wl,--gc-sections \
	-Wl,-Map,$(BUILDDIR)/$(BINARYNAME).map,--cref \
	$(MCU)  \
	-nostdlib \
	-nostartfiles \

DEPFLAGS = -MMD -MP -MF $(BUILDDIR)/$(basename $<).d

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

$(UIMAGENAME): $(BIN)
	$(UBOOTDIR)/tools/mkimage -A arm -C none -T kernel -a $(LOADADDR) -e $(ENTRYPOINT) -d $< $@

$(BUILDDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	$(AS) $(AFLAGS) $< -o $@ > $(addprefix $(BUILDDIR)/, $(addsuffix .lst, $(basename $<)))

$(BUILDDIR)/%.o: %.c $(BUILDDIR)/%.d
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	$(CC) -c $(DEPFLAGS) $(OPTFLAG) $(CFLAGS) $< -o $@

$(ELF): $(OBJECTS) $(LINKSCR)
	$(info Linking...)
	$(LD) $(LFLAGS) -o $@ $(OBJECTS)

$(BIN): $(ELF)
	$(OBJCPY) -O binary $< $@

%.d: ;

clean:
	rm -rf build

ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif

.PRECIOUS: $(DEPS) $(OBJECTS) $(ELF)
.PHONY: all

