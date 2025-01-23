#!/bin/sh
type=$1

if [ "$type" = "ioboard" ]
then
dev_info=$(sudo ls /dev/tty* | grep "ttyUSB*")
echo ${dev_info}
#echo ${dev_info#*ttyUSB}
else
IFS='/'; devices=($type); unset IFS;
dev_name=${devices[-1]}
camera=$(sudo ls /dev/v4l/by-path | grep "$dev_name")
echo ${camera}
fi
