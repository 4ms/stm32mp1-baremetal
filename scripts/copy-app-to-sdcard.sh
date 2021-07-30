#!/usr/bin/env bash

[ "$#" -eq 2 ] || { 
	echo ""
	echo "Usage:"
	echo "scripts/copy_to_sdcard.sh path/to/application.uimg /sdcard/mount-point/" >&2;
	echo ""
	echo "/sdcard/mount-point/ is where the SD card is mounted."
	echo ""
	echo "Example: scripts/copy_to_sdcard.sh examples/minimal_boot/build/a7-main.img /Volumes/BAREAPP/"
	echo ""
	exit 1;
}

if [ ! -f $1 ]; then
	echo "Application uimg file $1 does not exist";
	exit 1;
fi

if [ ! -d $2 ]; then 
	echo "Error: $2 does not exist."; 
	exit 1;
fi

case "$(uname -s)" in
	Darwin)
		UNMOUNTCMD="diskutil unmount $2"
		;;
	Linux)
		UNMOUNTCMD="unmount $2"
		;;
	*)
		echo 'OS not supported: disk was not unmounted'
		;;
esac

echo ""
echo "Original contents of SD card:"
echo "ls -l $2"
ls -l $2

echo ""
echo "Copying to SD card"
echo "cp $1 $2"
cp $1 $2

echo ""
echo "New contents of sdcard:"
ls -l $2

echo ""
echo "Unmounting sdcard..."
eval $UNMOUNTCMD

echo "Done!"
