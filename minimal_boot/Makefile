BINARYNAME = app
BUILDDIR = build
UIMAGENAME = $(BUILDDIR)/a7-main.uimg
LOADADDR 	= 0xC2000040
ENTRYPOINT 	= 0xC2000040

STARTUP = startup.s
LINKSCR = linkscript.ld

UBOOTDIR =../u-boot/build

SOURCES = $(STARTUP)

OBJECTS   = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))
DEPS   	  = $(addprefix $(BUILDDIR)/, $(addsuffix .d, $(basename $(SOURCES))))

INCLUDES = -I. \

MCU =  -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mlittle-endian -mfloat-abi=hard

ARCH_CFLAGS += -DSTM32MP157Cxx \
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
		-Wno-volatile \


LFLAGS = -Wl,--gc-sections \
	-Wl,-Map,$(BUILDDIR)/$(BINARYNAME).map,--cref \
	$(MCU)  \
	-T $(LINKSCR) \
	-nostdlib \
	-nostartfiles \
	-ffreestanding

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

$(BUILDDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(AS) $(AFLAGS) $< -o $@ > $(addprefix $(BUILDDIR)/, $(addsuffix .lst, $(basename $<)))

$(BUILDDIR)/%.o: %.c $(BUILDDIR)/%.d
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(CC) -c $(DEPFLAGS) $(OPTFLAG) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.cc $(BUILDDIR)/%.d
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(CXX) -c $(DEPFLAGS) $(OPTFLAG) $(CXXFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.cpp $(BUILDDIR)/%.d
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

