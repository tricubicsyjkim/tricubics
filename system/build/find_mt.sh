#!/bin/sh
dev_info=$(sudo cat /proc/bus/input/devices | grep -A 8 "Multi touch   Multi touch overlay device" | grep -B 4 "ABS=273800000000003" | grep "event")
#echo ${dev_info}
echo ${dev_info#*event}
