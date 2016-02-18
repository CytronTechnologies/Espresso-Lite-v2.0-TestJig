#!/usr/bin/python

import sys
import os
import serial
import time
import array
import serialESPCmd

def startSerial(serial_port):
  print 'opening port: ', serial_port
  port = serial.Serial(serial_port, baudrate=115200, timeout=3.0)
  return handlingSerialEvent(port)

def handlingSerialEvent(ser):
#  try:
    #data = array.array('i', (0 for x in range(0, 5)))
    #data[3] = 0x45
    cmd = serialESPCmd.retrieveInfo
    rpiSendCmd(ser, cmd)
    if rpiReceiveCorrect(ser) == chr(0x01):
      print 'Receive correct'
      val = rpiReceiveData(ser, cmd)
      print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
    else:
      print 'Error'
    
    #while True:
    #  inByte = ser.read()
    #  if inByte:
    #    val =  ord(inByte)
    #    print "%02X" % val
    #    if(inByte==chr(0x4d)):
    #      break
    print 'closing port: ', ser.port
    ser.close()
    return 0

  #except:
  #  if (ser.isOpen()==True):
  #   ser.close()
  #  return 1

def rpiSendCmd(*args):
  if len(args) == 2:
    _rpiSendCmd(args[0], args[1], [])
  elif len(args) == 3:
    _rpiSendCmd(args[0], args[1], args[2])
  else:
    raise TypeError

def _rpiSendCmd(ser, cmd, data):
  data_total_sum = 0
  checksum = 0
  packet_length = 1
  key = ''
  ser.write(chr(0x4e))
  ser.write(chr(0x5c))
  ser.write(chr(0x01))
  if data is not None:
    packet_length = packet_length + len(data)
    data_total_sum = sum(data)
    key = ''.join(chr(x) for x in data)
  ser.write(chr(packet_length))
  ser.write(chr(cmd))
  #if key != '':
  ser.write(key)
  checksum = (171 + packet_length + cmd + data_total_sum) % 256
  ser.write(chr(checksum))

def rpiReceiveCorrect(ser):
  inByteString = ''
  inByte = ser.read()
  if inByte == chr(0x4e):
    inByteString = ser.read(5)
    if len(inByteString) == 5:
     #print 'receive success'
     #print inByteString[3].encode('hex')
     return inByteString[3]
  return 0

def rpiReceiveData(ser, cmd):
  _flag= 0
  _packet_len = 0
  _cmd = 0
  _data = ''
  _checksum = 0
  inByteString = ser.read(2)
  if len(inByteString) == 2 and inByteString == '\x4e\x5c':
    inByteString = ser.read(2)
    if len(inByteString) == 2:
      _flag = ord(inByteString[0])
      _packet_len = ord(inByteString[1])
      inByteString = ser.read(_packet_len + 1)
      if len(inByteString) == _packet_len + 1 :
       _cmd = ord(inByteString[0])
       if _cmd != cmd:
         print 'incorrect match of command'
         return 0
       _data = inByteString[1:_packet_len]
       #print 'Date received: ' ,_data.encode('hex')
       _checksum = ord(inByteString[_packet_len])
       #print _checksum
       _convert_data = array.array('i', (ord(_data[x]) for x in range(0, len(_data))))
       _cal_checksum = (170 + _flag + _packet_len + _cmd + sum(_convert_data)) % 256
       #print _cal_checksum
       if _checksum != _cal_checksum:
         print 'checksum error'
         return 0
       return _convert_data
  return 0
  
def main(args):
  return startSerial(args[1])

if __name__ =='__main__':
  sys.exit(main(sys.argv))
