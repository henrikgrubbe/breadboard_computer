#!/usr/bin/python  
import serial as sl
import sys, time, fileinput

def progressbar(it):
    count = len(it)
    size = 32
    def show(j):
        x = int(size*j/count)
        sys.stdout.write("%s[%s%s] %i/%i\r" % ("Writing program: ", "#"*x, "."*(size-x), j, count))
        sys.stdout.flush()        
    show(0)
    for i, item in enumerate(it):
        yield item
        show(i+1)
    sys.stdout.write("\n")
    sys.stdout.flush()

ser = sl.Serial()
ser.dtr = False
ser.port = '/dev/ttyUSB0'
ser.baudrate = 115200
ser.timeout = 0.1
ser.open()

program = []
for l in fileinput.input():
    program.append(l)

for l in progressbar(program):
    addrOut = int(l.split(':')[0], 2)
    dataOut = int(l.split(':')[1][0:8], 2)

    success = False
    attempts = 0
    while not success:
        attempts += 1
        ser.write(l.encode('utf-8'))
        ser.flush()
        
        lineIn = ser.readline().decode('utf-8')
        if len(lineIn) > 0:
            addrIn = int(lineIn.split(':')[0])
            dataIn = int(lineIn.split(':')[1])
            success = addrIn == addrOut and dataIn == dataOut 

    if attempts >= 10:
        print('Failed to write ', addrOut, ':', dataOut, '... Aborting')
        exit()
