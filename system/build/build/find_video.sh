#!/bin/sh
type=$1

if [ "$type" = "ioboard" ]
then
dev_info=$(sudo ls /dev/tty* | grep "ttyUSB*")
#echo ${dev_info}
echo ${dev_info#*ttyUSB}
else
dev_name=$1
camera=$(sudo ls /dev/video* | grep "$dev_name")
echo ${camera}
fi
