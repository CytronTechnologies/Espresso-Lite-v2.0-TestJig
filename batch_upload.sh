#!/bin/bash

cd /
cd home/pi

#define variables here
dir=$(mktemp -d)
exit_value=0
max_dev=6
#

let max_dev=max_dev-1

sudo python msg_oled.py "Uploading program now..." "" "Program name:" "$1"

for i in `seq 0 $max_dev` 
do
  { sudo ./esptool -cd ck -cb 115200 -cp "/dev/ttyUSB$i" -ca 0x000000 -cf "./$1" ; echo "$?" > "$dir/$i" ;} &
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
   exit_value=1
  fi
  #echo "${!var}"
  #echo "$exit_value"
 done <"$file"
 ((val=val+1))
done

rm -r "$dir"

#display result
sudo python msg_oled.py "Upload completed" "" " 1 : $dev1     4 : $dev4" " 2 : $dev2     5 : $dev5" " 3 : $dev3     6 : $dev6"

cd /

if [ "$exit_value" -eq 0 ];then
  (exit 0)
else
  (exit 1)
fi
#exit 2
#echo $?
