#include "fake_uclass_dm.h"

int uclass_get_device(enum uclass_id id, int seq, struct udevice **devp)
{
	// call probe (?) on UCLASS_USB_GADGET_GENERIC, "usb"
	// create a udevice

	return 0;
}

int uclass_get_device_by_seq(enum uclass_id id, int seq, struct udevice **devp)
{
	return uclass_get_device(id, seq, devp);
}

int device_remove(struct udevice *dev, int flags)
{
	return 0;
}
