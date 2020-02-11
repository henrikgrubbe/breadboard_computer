#!/usr/bin/python  
import serial as sl  
import fileinput
import time

ser = sl.Serial('/dev/ttyUSB0',115200)

time.sleep(2)
ser.write("\n".encode('utf-8'))
for line in fileinput.input():
    ser.write(line.encode('utf-8'))
    time.sleep(.06)