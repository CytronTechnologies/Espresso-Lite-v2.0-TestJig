#!/bin/bash

filepath=/home/pi/TestJig
cd "$filepath"

#define variables here
dir="$filepath"/status
max_dev=6
#

let max_dev=max_dev-1

#sudo python msg_oled.py "Uploading program now..." "" "Program name:" "Espresso-Lite-TestJig" "-Board.ino.bin"

{ sudo ./esptool -cd ck -cb 115200 -cp "/dev/ttyUSBCT0" -ca 0x000000 -cf "./$1" > /dev/null ; echo "$?" > "$dir"/0 ;} &
{ sudo ./esptool -cd ck -cb 115200 -cp "/dev/ttyUSBCT1" -ca 0x000000 -cf "./$1" > /dev/null ; echo "$?" > "$dir"/1 ;} &
{ sudo ./esptool -cd ck -cb 115200 -cp "/dev/ttyUSBCT2" -ca 0x000000 -cf "./$1" > /dev/null; echo "$?" > "$dir"/2 ;} &
{ sudo ./esptool -cd ck -cb 115200 -cp "/dev/ttyUSBCT3" -ca 0x000000 -cf "./$1" > /dev/null ; echo "$?" > "$dir"/3 ;} &
{ sudo ./esptool -cd ck -cb 115200 -cp "/dev/ttyUSBCT4" -ca 0x000000 -cf "./$1" > /dev/null ; echo "$?" > "$dir"/4 ;} &
{ sudo ./esptool -cd ck -cb 115200 -cp "/dev/ttyUSBCT5" -ca 0x000000 -cf "./$1" > /dev/null ; echo "$?" > "$dir"/5 ;} &
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
sudo python msg_oled.py "Upload completed" "" " 1 : $dev1     4 : $dev4" " 2 : $dev2     5 : $dev5" " 3 : $dev3     6 : $dev6"

cd /
