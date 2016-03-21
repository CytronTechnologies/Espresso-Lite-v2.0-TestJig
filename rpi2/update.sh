#!/bin/bash

path1=/home/pi/GitHub/Espresso-Lite-v2.0-TestJig/rpi2/TestJig/_2000_ESPert_workshop.ino.bin
path2=/home/pi/TestJig/_2000_ESPert_workshop.ino.bin

wget -q --tries=10 --timeout=20 --spider http://google.com
if [[ $? -eq 0 ]]; then
        echo "Online"
        cd /home/pi/GitHub/*
	git pull
	cd
else
        echo "Offline"
        exit 0
fi

if diff "$path1" "$path2" ; then
   echo "Nothing changed"
else
   echo "Something changed"
   cp "$path1" /home/pi/TestJig
fi
