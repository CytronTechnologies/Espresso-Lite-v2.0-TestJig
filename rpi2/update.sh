#!/bin/bash

display=/home/pi/TestJig/msg_oled.py
path1=/home/pi/GitHub/Espresso-Lite-v2.0-TestJig/rpi2/TestJig/_2000_ESPert_workshop.ino.bin
path2=/home/pi/TestJig/_2000_ESPert_workshop.ino.bin
path3=/home/pi/GitHub/Espresso-Lite-v2.0-TestJig/rpi2/TestJig/Espresso-Lite-Testjig-small-rpi2.ino.bin
path4=/home/pi/TestJig/Espresso-Lite-Testjig-small-rpi2.ino.bin
path5=/home/pi/GitHub/Espresso-Lite-v2.0-TestJig/rpi2/TestJig/Espresso-Lite-Testjig-small-rpi2-asus.ino.bin
path6=/home/pi/TestJig/Espresso-Lite-Testjig-small-rpi2-asus.ino.bin
path7=/home/pi/GitHub/Espresso-Lite-v2.0-TestJig/rpi2/TestJig/ESPert_testjig_main_small.py
path8=/home/pi/TestJig/ESPert_testjig_main_small.py
isChanged=0

wget -q --tries=10 --timeout=20 --spider http://google.com
if [[ $? -eq 0 ]]; then
        echo "Online"
        sudo python "$display" "Updating..."
        cd /home/pi/GitHub/*
	git pull
	cd
else
        echo "Offline"
        exit 0
fi

if diff "$path3" "$path4" ; then
   sudo python "$display" "No change to rpi2"
else
   cp "$path3" /home/pi/TestJig
   sudo python "$display" "Copy rpi2"
   isChanged=1
fi

if diff "$path5" "$path6" ; then
   sudo python "$display" "No change to rpi2-asus"
else
   cp "$path5" /home/pi/TestJig
   sudo python "$display" "Copy rpi2-asus"
   isChanged=1
fi

if diff "$path7" "$path8" ; then
   sudo python "$display" "No change to main"
else
   cp "$path7" /home/pi/TestJig
   sudo python "$display" "Copy main"
   isChanged=1
fi

if diff "$path1" "$path2" ; then
   echo "Nothing changed"
   sudo python "$display" "No program change"
else
   echo "Something changed"
   cp "$path1" /home/pi/TestJig
   sudo python "$display" "Update complete"
   isChanged=1
   
fi

if [ $isChanged -eq 1 ];then
   sleep 1
   echo "reboot now"
   sudo python "$display" "Rebooting now..."
   reboot
   exit 0
fi

exit 0
