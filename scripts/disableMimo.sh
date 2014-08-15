#!/bin/bash
rm /etc/X11/xorg.conf
sudo cp -f /boot/cmdline-orig.txt /boot/cmdline.txt
sudo reboot