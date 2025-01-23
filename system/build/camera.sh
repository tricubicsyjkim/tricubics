#!/bin/sh
dev_info=$(sudo ls /dev/tty* | grep "ttyUSB")
echo ${dev_info}
#echo ${dev_info#*event}
