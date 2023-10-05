BINARYNAME := plugin
BUILDDIR := build

OPTFLAG := -O0

SHAREDDIR := ../../../shared
SCRIPTDIR := ../../../scripts

SOURCES := pluginC.cc \
		  calc.cc \
		  init.cc \

INCLUDES := -I. \

LINKSCR := linkscript.ld

MCU :=  -mcpu=cortex-a7 -march=armv7ve -mfpu=neon-vfpv4 -mlittle-endian -mfloat-abi=hard

ARCH_CFLAGS := -DSTM32MP157Cxx \
			   -DSTM32MP1 \
			   -DCORE_CA7 \

CFLAGS = $(MCU) \
		 $(ARCH_CFLAGS) \
		 $(INCLUDES) \
		 -fno-common \
		 -fdata-sections -ffunction-sections \
		 -nostdlib -nostartfiles -ffreestanding \


CXXFLAGS = $(CFLAGS) \
		   -std=c++2a \
		   -fno-rtti \
		   -fno-exceptions \
		   -fno-unwind-tables \
		   -fno-threadsafe-statics \
		   -mno-unaligned-access \

LFLAGS = -Wl,--gc-sections \
		 -Wl,-Map,$(BUILDDIR)/$(BINARYNAME).map,--cref \
		 $(MCU)  \
		 -nostdlib \
		 -nostartfiles \
		 -ffreestanding \

#-fomit-frame-pointer

%.diss : %.elf
	arm-none-eabi-objdump -Dz --source $^ > $@

#### Shared lib (-shared), single file ##################

extcall: $(BUILDDIR)/plugin-extcall.readelf $(BUILDDIR)/plugin-extcall.diss

$(BUILDDIR)/plugin-extcall.so: plugin-extcall.cc
	@mkdir -p $(OBJDIR)
	$(CXX) $(DEPFLAGS) $(OPTFLAG) $(CXXFLAGS) -shared -fPIC $< -o $@


%.diss : %.so
	arm-none-eabi-objdump -CDz --source $^ > $@

%.nm : %.so
	arm-none-eabi-nm -CA $^ > $@

%.readelf : %.so
	arm-none-eabi-readelf -C -a $^ > $@



#### Shared lib (-shared), multiple files ##################

extcall-multi: $(BUILDDIR)/plugin-extcall-multi.readelf $(BUILDDIR)/plugin-extcall-multi.diss

$(BUILDDIR)/plugin-extcall-multi.so: plugin-extcall-multifile1.cc plugin-extcall-multifile2.cc
	@mkdir -p $(OBJDIR)
	$(CXX) $(DEPFLAGS) $(OPTFLAG) -c $(CXXFLAGS) plugin-extcall-multifile1.cc -o $(BUILDDIR)/plugin-extcall-multifile1.o
	$(CXX) $(DEPFLAGS) $(OPTFLAG) -c $(CXXFLAGS) plugin-extcall-multifile2.cc -o $(BUILDDIR)/plugin-extcall-multifile2.o
	$(LD) -shared $(LFLAGS) -o $@ $(BUILDDIR)/plugin-extcall-multifile1.o $(BUILDDIR)/plugin-extcall-multifile2.o




# got_plt: CFLAGS += -shared -fPIC 
# got_plt: $(BUILDDIR)/$(BINARYNAME).diss
	
# pie1: CFLAGS += -c -fPIC -msingle-pic-base 
# pie1: LFLAGS += -fPIC -Wl,--unresolved-symbols=ignore-in-object-files -Wl,--emit-relocs
# pie1: $(BUILDDIR)/$(BINARYNAME).so

# got_only: CFLAGS += -c -fPIC 
# got_only: LFLAGS += -c -fPIC
# .PHONY: got_only
# got_only: $(BUILDDIR)/obj/obj/pluginA.o


# got_plt: CFLAGS = -shared -fPIC
# got_plt: LFLAGS += -shared -fPIC



include $(SHAREDDIR)/makefile-common.mk


