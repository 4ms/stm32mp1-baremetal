# Makefile by Dan Green <danngreen1@gmail.com>
	
export BINARYNAME = main
export BUILDDIR_A7 = build/corea7
export BUILDDIR_M4 = build/corem4
export LIBDIR = ../lib

all: a7 m4

_embedm4:
	@$(MAKE) --no-print-directory -f makefile_mp1m4.mk
	@# Convert .elf to .bin for each section
	@# Note: maybe can use objcopy to convert sections to .o?
	arm-none-eabi-objcopy \
		--dump-section .text=$(BUILDDIR_M4)/firmware.bin \
		--dump-section .isr_vector=$(BUILDDIR_M4)/vectors.bin \
		$(BUILDDIR_M4)/$(BINARYNAME).elf
	arm-none-eabi-objcopy -I binary -B armv7 -O elf32-littlearm $(BUILDDIR_M4)/firmware.bin $(BUILDDIR_M4)/firmware_m4.o
	arm-none-eabi-objcopy -I binary -B armv7 -O elf32-littlearm $(BUILDDIR_M4)/vectors.bin $(BUILDDIR_M4)/firmware_m4_vectors.o
	@# Convert .bin files to c arrays
	@#xxd -i $(BUILDDIR_M4)/firmware.bin firmware_m4.c
	@#xxd -i $(BUILDDIR_M4)/vectors.bin firmware_m4_vectors.c
	@# Build a7 firmware, which converts firmware_m4*.c files to .o files
	@# and links them into the proper sections
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk all

embedm4:
	@$(MAKE) --no-print-directory -f makefile_mp1m4.mk
	arm-none-eabi-objcopy \
		--dump-section .text=$(BUILDDIR_M4)/text.bin \
		--dump-section .rodata=$(BUILDDIR_M4)/rodata.bin \
		--dump-section .isr_vector=$(BUILDDIR_M4)/vectors.bin \
		$(BUILDDIR_M4)/$(BINARYNAME).elf
	cat $(BUILDDIR_M4)/text.bin >> $(BUILDDIR_M4)/firmware.bin
	cat $(BUILDDIR_M4)/rodata.bin >> $(BUILDDIR_M4)/firmware.bin
	xxd -i $(BUILDDIR_M4)/firmware.bin firmware_m4.h
	xxd -i $(BUILDDIR_M4)/vectors.bin firmware_m4_vectors.h
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk all

a7:
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk all image

m4:
	@$(MAKE) --no-print-directory -f makefile_mp1m4.mk

combo: a7 m4
	@ cat $(BUILDDIR_M4)/$(BINARYNAME).hex $(BUILDDIR_A7)/$(BINARYNAME).hex | \
	awk 'BEGIN {} END {print ":00000001FF"} /^:00000001FF/{$$0 = ""} {if ($$0 != ""){ print } }' > build/combo.hex
	@ ls -l build/combo.hex

clean:
	rm -rf $(BUILDDIR_A7)
	rm -rf $(BUILDDIR_M4)
