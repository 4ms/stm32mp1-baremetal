# If you want to use `make flash`, set this to the path to your JFlash executable:
JFLASHEXE = /Applications/SEGGER/JLink_V696/JFlash.app/Contents/MacOS/JFlashExe

export BUILDDIR_H7M7 	= build/h7corem7
export BUILDDIR_H7M4 	= build/h7corem4
export BUILDDIR_MP1A7 	= build/mp1corea7
export BUILDDIR_MP1M4 	= build/mp1corem4
export BINARYNAME 		= main
export LIBDIR 			= lib
export DEVICEBASE		= $(LIBDIR)/cmsis-device
export CMSIS 			= $(LIBDIR)/cmsis
export HALBASE 			= $(LIBDIR)/stm32-hal
export DRIVERLIB       	= $(LIBDIR)/mdrivlib
export LINKSCRIPTDIR   	= system/linker
export SHARED  			= ../shared
export TABLEGEN 		= $(SHARED)/tableGen

all: 
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk all image
	@$(MAKE) --no-print-directory -f makefile_mp1m4.mk
	@$(MAKE) --no-print-directory -f makefile_h7m7.mk
	@$(MAKE) --no-print-directory -f makefile_h7m4.mk
	@$(MAKE) --no-print-directory tests

a7:
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk all image

mp1m4:
	@$(MAKE) --no-print-directory -f makefile_mp1m4.mk

h7:
	@$(MAKE) --no-print-directory -f makefile_h7m4.mk
	@$(MAKE) --no-print-directory -f makefile_h7m7.mk

inttest: 
	touch src/patchlist.cc 
	touch src/patchlist.hh
	@$(MAKE) --no-print-directory EXTDEF=METAMODULE_INTEGRATION_TESTS all

clean:
	rm -rf $(BUILDDIR_H7M4)
	rm -rf $(BUILDDIR_H7M7)
	rm -rf $(BUILDDIR_MP1A7)
	rm -rf $(BUILDDIR_MP1M4)

clean_a7:
	$(MAKE) --no-print-directory clean -f makefile_mp1a7.mk

clean_uboot:
	$(MAKE) --no-print-directory clean_uboot -f makefile_mp1a7.mk


$(BUILDDIR_H7M4)/$(BINARYNAME).hex:
	@$(MAKE) --no-print-directory -f makefile_m4.mk

$(BUILDDIR_H7M7)/$(BINARYNAME).hex:
	@$(MAKE) --no-print-directory -f makefile_m7.mk


# ifeq (,$(wildcard $(JFLASHEXE)))
#     $(error JFlashExe program not found at $(JFLASHEXE). Please edit this Makefile and enter the correct path)
# endif

flash_a7:
	$(info SD card must be mounted)
	cp $(BUILDDIR_MP1M4)/$(BINARYNAME).elf /Volumes/BAREAPP/m4-main.elf
	cp $(BUILDDIR_MP1A7)/$(BINARYNAME).uimg /Volumes/BAREAPP/a7-main.uimg
	diskutil unmount /Volumes/BAREAPP

flash_mp1m4:
	$(JFLASHEXE) -openprjflashing/metamodule-mp1-cm4.jflash -open$(BUILDDIR_MP1M4)/$(BINARYNAME).hex -auto -exit

flash_h7: $(BUILDDIR_H7M7)/$(BINARYNAME).hex $(BUILDDIR_M4)/$(BINARYNAME).hex
	$(JFLASHEXE) -openprjflashing/metamodule-h7-cm7.jflash -open$(BUILDDIR_H7M7)/$(BINARYNAME).hex -auto -open$(BUILDDIR_M4)/$(BINARYNAME).hex -auto -exit

start_gdb:
	JLinkGDBServer -select USB -device STM32MP15xx_M4_Bank1 -endian little -if SWD -speed 8000 -noir -nohalt

tables $(SHARED)/util/math_tables.cc: $(TABLEGEN)/main.cpp
	g++ -I../shared -std=c++17 $(TABLEGEN)/main.cpp -o $(TABLEGEN)/make_tables
	cd $(TABLEGEN) && ./make_tables

simulate:
	$(info Simulating screens...)
	@cd ../screen_simulator && cargo run

.PHONY: all clean flash_a7 flash_m7 flash_m4 flash_h7 tables tests hardware_tests core_tests util_tests proc_tests mdrivlib_tests simulate

tests: module_tests core_tests util_tests mdrivlib_tests #vcv_tests

module_tests:
	$(info ... Running firmware tests....)
	@$(MAKE) --no-print-directory -f tests/Makefile

mdrivlib_tests:
	$(info ... Running mdrivlib tests ...)
	@cd $(DRIVERLIB) && $(MAKE) --no-print-directory -f tests/Makefile

util_tests:
	$(info ... Running shared/util tests ...)
	@cd $(SHARED)/util && $(MAKE) --no-print-directory -f tests/Makefile

core_tests:
	$(info ... Running CoreModule tests ...)
	@cd $(SHARED)/CoreModules && $(MAKE) --no-print-directory -f tests/Makefile


generate_compile_txt:
	arm-none-eabi-gcc -E -x c++ - -v < /dev/null 2>&1 | \
    awk '/End of search list./ { show=0 } { if (show) printf "-I%s\n",$1 }; /#include <...> search starts here:/ { show=1; }' > compile_flags.txt

#https://github.com/nickdiego/compiledb
compile_commands:
	compiledb make
	compdb -p ./ list > compile_commands.tmp 2>/dev/null
	rm compile_commands.json
	mv compile_commands.tmp compile_commands.json
