#!/bin/sh -e
#

# Wait for the TV-screen to be turned on...
while ! $( tvservice --dumpedid /tmp/edid | fgrep -qv 'Nothing written!' ); do
	bHadToWaitForScreen=true;
	printf "===> Screen is not connected, off or in an unknown mode, waiting for it to become available...\n"
	sleep 10;
done;

# Get screen resolution
printf "===> Screen is on, extracting preferred mode...\n"
_DEPTH=32;
eval $( edidparser /tmp/edid | fgrep 'preferred mode' | tail -1 | sed -Ene 's/^.+(DMT|CEA) \(([0-9]+)\) ([0-9]+)x([0-9]+)[pi]? @.+/_GROUP=\1;_MODE=\2;_XRES=\3;_YRES=\4;/p' );

# Check to see if were on the mimo
if [ $_XRES -eq 800 ]; then
	if [ -e /etc/X11/xorg.conf ]; then
		echo "Already setup for mimo"
	else
		echo "Mimo detected. Copying xorg files"
		cp -f /etc/X11/xorg-mimo.conf /etc/X11/xorg.conf
		sudo cp -f /boot/cmdline-touch.txt /boot/cmdline.txt
		echo "Rebooting"
		sudo reboot
	fi
else
	if [ -e /etc/X11/xorg.conf ]; then
		echo "Monitor detected. Removing xorg files"
		rm -f /etc/X11/xorg.conf
		sudo cp -f /boot/cmdline-orig.txt /boot/cmdline.txt
		echo "Rebooting"
		sudo reboot
	else
		echo "Already setup for monitors"
	fi
fi
