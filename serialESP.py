#!/usr/bin/python

import sys
import os
import serial
import time
#import ctypes, ctypes.util

#clib = ctypes.cdll.LoadLibrary(ctypes.util.find_library('c'))

def startSerial(serial_port):
  print 'opening port: ', serial_port
  port = serial.Serial(serial_port, baudrate=115200, timeout=1.0)
  return handlingSerialEvent(port)

def handlingSerialEvent(ser):
#  try:
    ser.write(chr(0xff))
    while True:
      inByte = ser.read()
      if inByte:
        val =  ord(inByte)
        print "%02X" % val
        if(inByte==chr(0x4d)):
          break
    print 'closing port: ', ser.port
    ser.close()
    return 0

  #except:
  #  if (ser.isOpen()==True):
  #   ser.close()
  #  return 1

def main(args):
  return startSerial(args[1])

if __name__ =='__main__':
  sys.exit(main(sys.argv))
