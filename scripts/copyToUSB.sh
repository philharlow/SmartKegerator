#!/bin/bash
zenity --info --text 'Starting transfer'
sudo mv /home/pi/Desktop/SmartKegerator-build-desktop-Qt_4_8_2__System__Debug/SmartKegerator /media/1GB\ FLASH/debug/
sudo mv /home/pi/Desktop/SmartKegerator-build-desktop-Qt_4_8_2__System__Release/SmartKegerator /media/1GB\ FLASH/release/
zenity --info --text 'Moves competed'