#!/usr/bin/python  
import serial as sl  
import fileinput
import time

ser = sl.Serial()
ser.dtr = False
ser.port = '/dev/ttyUSB0'
ser.baudrate = 115200
ser.open()

time.sleep(2)
ser.write("\n".encode('utf-8'))
for line in fileinput.input():
    ser.write(line.encode('utf-8'))
    time.sleep(.06)
