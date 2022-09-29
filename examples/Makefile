# Builds/re-builds all example projects

examples := \
	audio_processor \
	basic_irq \
	copro_embedded \
	copro_rproc \
	ctest \
	hal_lib \
	minimal_boot \
	multicore_a7 \
	nested_irq \
	usb_msc_device \

define build_example
echo
echo "*******************************"
echo "*** Building $1"
echo "*******************************"
make -C $1 clean all;
endef

.PHONY: all
all:
	@$(foreach example,$(examples),$(call build_example,$(example)))

