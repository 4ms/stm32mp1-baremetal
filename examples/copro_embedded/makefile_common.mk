# (c) 2021 Dan Green

ARCH 	?= arm-none-eabi
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

OBJDIR = $(BUILDDIR)/obj
OBJECTS   = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(SOURCES))))
DEPS   	  = $(addprefix $(OBJDIR)/, $(addsuffix .d, $(basename $(SOURCES))))

EXTRA_OBJECTS ?= 

OBJECTS += $(EXTRA_OBJECTS)

CFLAGS = -g2 -fno-common \
	$(ARCH_CFLAGS) $(MCU) \
	$(INCLUDES) \
	-fdata-sections -ffunction-sections \
	-nostdlib \
	-nostartfiles \
	$(EXTRA_CFLAGS)

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

AFLAGS = $(MCU)

LFLAGS = -Wl,--gc-sections \
	-Wl,-Map,$(BUILDDIR)/$(BINARYNAME).map,--cref \
	$(MCU)  \
	-T $(LOADFILE) \
	-nostdlib \
	-nostartfiles \
	-ffreestanding \

DEPFLAGS = -MMD -MP -MF $(OBJDIR)/$(basename $<).d

all: Makefile $(BIN) $(HEX)

install-mp1-boot:
	@if [ "$${SD_DISK_DEVPART}" = "" ]; then echo "Please specify the disk and partition like this: make install-mp1-boot SD_DISK_DEVPART=/dev/diskXs3"; \
	else \
	echo "sudo dd if=${UIMAGENAME} of=$${SD_DISK_DEVPART}" && \
	sudo dd if=${UIMAGENAME} of=$${SD_DISK_DEVPART};  fi

$(BIN): $(ELF)
	@$(OBJCPY) -O binary $< $@
	@$(OBJDMP) -x --syms $< > $(addsuffix .dmp, $(basename $<))
	ls -l $@ $<

$(HEX): $(ELF)
	@$(OBJCPY) --output-target=ihex $< $@
	@$(SZ) $(SZOPTS) $(ELF)

$(ELF): $(OBJECTS) $(LOADFILE)
	$(info Linking...)
	$(LD) $(LFLAGS) -o $@ $(OBJECTS)

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

$(OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(info Building $< at $(OPTFLAG))
	@$(AS) $(AFLAGS) $< -o $@ > $(addprefix $(BUILDDIR)/, $(addsuffix .lst, $(basename $<)))

%.d: ;

clean:
	rm -rf $(BUILDDIR)


ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif

.PRECIOUS: $(DEPS) $(OBJECTS) $(ELF) $(PRECHDRS)
.PHONY: all clean install install-mp1-boot

.PHONY: compile_commands
compile_commands:
	compiledb make
	compdb -p ./ list > compile_commands.tmp 2>/dev/null
	rm compile_commands.json
	mv compile_commands.tmp compile_commands.json
