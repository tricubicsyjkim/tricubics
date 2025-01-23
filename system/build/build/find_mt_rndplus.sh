#!/bin/sh
dev_info=$(sudo cat /proc/bus/input/devices | grep -A 8 "RNDPLUS PulseIR Touchscreen" | grep -B 4 "ABS=273800000000003" | grep "event")
#echo "line"
#echo $dev_info
#echo ${dev_info}
echo ${dev_info#*event}
