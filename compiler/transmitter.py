#!/usr/bin/python  
import serial as sl
import sys, time, fileinput

def showProgress(length, i, j):
    symbols = "|/-\\"
    x = int(32*i/length)
    if (i == length):
        sys.stdout.write("%s[%s%s] %i/%i\r" % ("Writing program: ", "#"*x, "."*(32-x), i , length))
    else:
        sys.stdout.write("%s[%s%s%s] %i/%i\r" % ("Writing program: ", "#"*(x-1), symbols[j % 4], "."*(32-x), i , length))

    sys.stdout.flush()

ser = sl.Serial()
ser.dtr = False
ser.port = '/dev/ttyUSB0'
ser.baudrate = 115200
ser.timeout = 0.5
ser.open()

program = []
for l in fileinput.input():
    program.append(l)

numLines = len(program)

showProgress(numLines, 0, 0)
for i, line, in enumerate(program):   
    addrOut = int(line.split(':')[0], 2)
    dataOut = int(line.split(':')[1][0:8], 2)

    success = False
    attempts = 0
    while not success:
        attempts += 1
        showProgress(numLines, i + 1, attempts)

        ser.write(line.encode('utf-8'))
        ser.flush()
        byteIn = ser.readline()
        lineIn = byteIn.decode('utf-8')

        if len(lineIn) > 0:
            addrIn = int(lineIn.split(':')[0])
            dataIn = int(lineIn.split(':')[1])
            success = addrIn == addrOut and dataIn == dataOut 

    if attempts >= 10:
        sys.stdout.write("\n")
        sys.stdout.flush()
        print('Failed to write ', addrOut, ':', dataOut, '... Aborting')
        exit()

sys.stdout.write("\n")
sys.stdout.flush()
