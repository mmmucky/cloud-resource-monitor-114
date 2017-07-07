#!/usr/bin/python

import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 9600)
time.sleep(1)
ser.setDTR(level=0)
time.sleep(1)

try:
    while True:
        ser.write('g')
        ser.write('y')
        ser.write('r')
        ser.write('b')
        time.sleep(5)
except:
    print('Caught Exception.  Exiting...')

ser.close()
