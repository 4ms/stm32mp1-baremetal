#include <dm/uclass-id.h>

struct udevice; // defined in dm/device.h

int uclass_get_device_by_seq(enum uclass_id id, int seq, struct udevice **devp);
int uclass_get_device(enum uclass_id id, int seq, struct udevice **devp);

#define DM_REMOVE_NORMAL 0
int device_remove(struct udevice *dev, int flags);
