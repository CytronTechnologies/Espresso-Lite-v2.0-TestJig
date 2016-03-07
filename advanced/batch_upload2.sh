#!/bin/bash

filepath=/home/pi/TestJig
cd "$filepath"

#define variables here
dir="$filepath"/status
#

sudo python msg_oled.py "Uploading program now..." "" "Program name:" "_2000_ESPert_" "workshop.ino.bin"

dev0="Pass"
dev1="Pass"
dev2="Pass"
dev3="Pass"
dev4="Pass"
dev5="Pass"
val=0

for file in "$dir"/*; do
 while IFS= read -r line ;do
  var="dev$val"
  if [ "$line" -eq "255" ];then
   declare "$var"="Fail" 
  fi
  echo "${!var}"
 done <"$file"
 ((val=val+1))
done

for i in `seq 0 5`
do
 var="dev$i"
 if [ "${!var}" = "Pass" ]; then
 { sudo ./esptool -cd ck -cb 115200 -cp "/dev/ttyUSB$i" -ca 0x000000 -cf "$1" ; echo "$?" > "$dir/$i" ;} &
 fi
done

wait

val=0

for file in "$dir"/* ;do
 while IFS= read -r line
 do
  var="dev$val"
  if [ "$line" -eq "255" ]; then
   declare "$var"="Fail"
  fi
 done <"$file"
 ((val=val+1))
done

#display result
sudo python msg_oled.py "Upload completed" "" " 1 : $dev0     4 : $dev3" " 2 : $dev1     5 : $dev4" " 3 : $dev2     6 : $dev5"

cd /
