# Makefile by Dan Green <danngreen1@gmail.com>
	
export BINARYNAME = main
export BUILDDIR_A7 = build/corea7
export BUILDDIR_M4 = build/corem4
export EXTLIBDIR = ../../third-party
export SHAREDDIR = ../../shared

# This compiles and links the m4 firmware to an .elf file,
# then splits up and re-combines the sections based on contigious memory blocks,
# and converts those binary files to .h files containing C-style arrays.
# Then, it builds the a7 firmware, which loads the arrays into the proper memory
# locations for running the m4 firmware. Whan a7 is ready, it resets and runs the m4 firmware.
# Todo: Find a better way of dumping and combining sections.
# Todo: Use the linker to directly embed m4 sections into the right place -- but we'll need
# to make sure u-boot knows how to translate those addresses in the .uimg file correctly.

all: firmware_m4.h firmware_m4_vectors.h
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk all 

$(BUILDDIR_M4)/$(BINARYNAME).elf: m4

firmware_m4.h: $(BUILDDIR_M4)/firmware.bin
	xxd -i $< $@

firmware_m4_vectors.h: $(BUILDDIR_M4)/vectors.bin
	xxd -i $< $@

$(BUILDDIR_M4)/vectors.bin: $(BUILDDIR_M4)/$(BINARYNAME).elf
	arm-none-eabi-objcopy --dump-section .isr_vector=$(BUILDDIR_M4)/vectors.bin $<

$(BUILDDIR_M4)/firmware.bin: $(BUILDDIR_M4)/$(BINARYNAME).elf
	arm-none-eabi-objcopy --dump-section .text=$(BUILDDIR_M4)/text.bin --dump-section .rodata=$(BUILDDIR_M4)/rodata.bin $<
	cat $(BUILDDIR_M4)/text.bin >> $@
	cat $(BUILDDIR_M4)/rodata.bin >> $@

install:
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk install

install-mp1-boot:
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk install-mp1-boot

clean:
	rm -rf $(BUILDDIR_A7)
	rm -rf $(BUILDDIR_M4)


# Below here isn't used by this project, but it's another way to create a file with a7 and m4 firmware.
# Run `make combo` and it'll produce a combo.hex file, which you can load using a flasher.
# Resetting and starting the m4 copro will still need to be done by the a7, but the a7 won't need to copy fw data.
# Then you could convert the combo.hex to a .bin and then .uimg file, which u-boot can load.
a7:
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk all 

m4:
	@$(MAKE) --no-print-directory -f makefile_mp1m4.mk


$(BUILDDIR_A7)/$(BINARYNAME).hex:
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk $@

$(BUILDDIR_M4)/$(BINARYNAME).hex:
	@$(MAKE) --no-print-directory -f makefile_mp1m4.mk $@

combo: a7 m4 $(BUILDDIR_A7)/$(BINARYNAME).hex $(BUILDDIR_M4)/$(BINARYNAME).hex
	@ cat $(BUILDDIR_M4)/$(BINARYNAME).hex $(BUILDDIR_A7)/$(BINARYNAME).hex | \
	awk 'BEGIN {} END {print ":00000001FF"} /^:00000001FF/{$$0 = ""} {if ($$0 != ""){ print } }' > build/combo.hex
	@ ls -l build/combo.hex

