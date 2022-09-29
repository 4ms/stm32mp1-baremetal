export BINARYNAME = main
export BUILDDIR_A7 = build/corea7
export BUILDDIR_M4 = build/corem4
export EXTLIBDIR = ../../third-party
export SHAREDDIR = ../../shared

all: a7 m4

a7:
	@$(MAKE) --no-print-directory -f makefile_mp1a7.mk all 

m4:
	@$(MAKE) --no-print-directory -f makefile_mp1m4.mk $(BUILDDIR_M4)/$(BINARYNAME).elf

clean:
	rm -rf $(BUILDDIR_A7)
	rm -rf $(BUILDDIR_M4)
