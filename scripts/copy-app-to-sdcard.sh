#!/usr/bin/env bash

[ "$#" -eq 2 ] || { 
	echo "Usage:"
	echo "scripts/copy_to_sdcard.sh path/to/application.uimg /dev/XXX" >&2;
	echo ""
	echo "Where /dev/XXX is the name of the 4th partition, e.g. /dev/disk1s4 or /dev/sdc4"
	echo ""
	exit 1;
}

if [ ! -f $1 ]; then
	echo "Application uimg file $1 does not exist";
	exit 1;
fi

if [ ! -b $2 ]; then 
	echo "Error: Device $2 does not exist."; 
	exit 1;
fi

echo ""
echo "Device $2 found, unmounting..."
sudo umount $2 || diskutil unmountVolume $2

echo ""
echo "Mounting on /tmp/sdcard_root"
rm -rf /tmp/sdcard_root
mkdir -p /tmp/sdcard_root
sudo mount -o user $2 /tmp/sdcard_root/

echo ""
echo "Current contents of sdcard:"
echo "ls -l /tmp/sdcard_root/"
ls -l /tmp/sdcard_root/

echo "Deleting bare-arm.uimg"
echo "sudo rm /tmp/sdcard_root/bare-arm.uimg"
sudo rm /tmp/sdcard_root/bare-arm.uimg

echo "Copying $1 to bare-arm.uimg on sdcard"
echo "sudo cp $1 /tmp/sdcard_root/bare-arm.uimg"
sudo cp $1 /tmp/sdcard_root/bare-arm.uimg

echo "New contents of sdcard:"
ls -l /tmp/sdcard_root/

echo "Unmounting sdcard..."
sudo umount $2 || diskutil unmountVolume $2

echo "Done!"
