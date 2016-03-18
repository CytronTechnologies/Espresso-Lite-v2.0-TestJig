#!/bin/bash

path1=GitHub/Espresso-Lite-v2.0-TestJig/rpi2/TestJig/_2000_ESPert_workshop.ino.bin
path2=TestJig/_2000_ESPert_workshop.ino.bin

cd /home/pi/GitHub/*
git pull
cd

if diff "$path1" "$path2" ; then
   echo "Nothing changed"
else
   echo "Something changed"
   cp "$path1" ./TestJig
fi
