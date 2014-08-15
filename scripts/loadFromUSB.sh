#!/bin/bash
zenity --info --text 'Transfer started!'
sudo mv /media/1GB\ FLASH/debug/SmartKegerator /home/pi/Desktop/SmartKegerator-build-desktop-Qt_4_8_2__System__Debug/
sudo mv /media/1GB\ FLASH/release/SmartKegerator /home/pi/Desktop/SmartKegerator-build-desktop-Qt_4_8_2__System__Release/
sudo chmod 777 /home/pi/Desktop/SmartKegerator-build-desktop-Qt_4_8_2__System__Debug/SmartKegerator
sudo chmod 777 /home/pi/Desktop/SmartKegerator-build-desktop-Qt_4_8_2__System__Release/SmartKegerator
zenity --info --text 'Copy comleted!'