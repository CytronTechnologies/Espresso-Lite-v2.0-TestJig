#!/bin/bash

filepath=/home/pi/TestJig
cd "$filepath"

#define variables here
dir="$filepath"/status
max_dev=6
#

let max_dev=max_dev-1

sudo python msg_oled.py "Testing modules now..."

for i in `seq 0 $max_dev` 
do
  { sudo python ./serialESP.py "/dev/ttyUSB$i"; echo "$?" > "$dir/$i" ;} &
done

wait

dev1="Pass"
dev2="Pass"
dev3="Pass"
dev4="Pass"
dev5="Pass"
dev6="Pass"
val=1

for file in "$dir"/*
do
 while IFS= read -r line
 do
  var="dev$val"
  if [ "$line" -eq "255" ];then
   declare "$var"="Fail"
  fi
  #echo "${!var}"
 done <"$file"
 ((val=val+1))
done

#display result
sudo python msg_oled.py "All test completed" "" " 1 : $dev1     4 : $dev4" " 2 : $dev2     5 : $dev5" " 3 : $dev3     6 : $dev6"

cd /
