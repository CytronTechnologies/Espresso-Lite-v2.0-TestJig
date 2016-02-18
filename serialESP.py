#!/usr/bin/python

import sys
import os
import serial
import time
import array
import serialESPCmd

board_id = 0

def startSerial(serial_port):
  print 'opening port: ', serial_port
  port = serial.Serial(serial_port, baudrate=115200, timeout=3.0)
  return handlingSerialEvent(port)

def handlingSerialEvent(ser):
  retErr = 0
  try:
#Testphase 1 - Retrieve Info
    cmd = serialESPCmd.retrieveInfo
    rpiSendCmd(ser, cmd)
    if rpiReceiveCorrect(ser) == serialESPCmd.receiveCorrect:
      print 'Receive correct'
      isErr, val = rpiReceiveData(ser, cmd)
      if not isErr and val[0]==serialESPCmd.operationSuccessful:
        print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
        isErr, val = rpiReceiveData(ser, cmd)
        if not isErr and len(val) == 3:
          print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
          board_id = sum([ (val[x]<<(x<<3)) for x in range(0, len(val)) ])
          print board_id
        else:
          print 'Fail to retrieve board ID'
          retErr = 1
          serialEventExit(ser)
          return retErr
      else:
        print 'Timeout'
        retErr = 1
        serialEventExit(ser)
        return retErr
    else:
      print 'Error'
      retErr = 1
      serialEventExit(ser)
      return retErr

#Testphase 2 - Check module WiFi connection
    cmd = serialESPCmd.checkWiFiConnection
    rpiSendCmd(ser, cmd)
    if rpiReceiveCorrect(ser) == serialESPCmd.receiveCorrect:
      print 'Receive correct'
      isErr, val = rpiReceiveData(ser, cmd, 10)
      if not isErr and val[0]==serialESPCmd.operationSuccessful:
        print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
      else:
        print 'Timeout'
        retErr = 1
        serialEventExit(ser)
        return retErr
    else:
      print 'Error'
      retErr = 1
      serialEventExit(ser)
      return retErr

#Testphase 3 - Client Test
    cmd = serialESPCmd.clientTest
    rpiSendCmd(ser, cmd)
    if rpiReceiveCorrect(ser) == serialESPCmd.receiveCorrect:
      print 'Receive correct'
      isErr, val = rpiReceiveData(ser, cmd)
      if not isErr and val[0]==serialESPCmd.operationSuccessful:
        print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
      else:
        print 'Timeout'
        retErr = 1
        serialEventExit(ser)
        return retErr
    else:
      print 'Error'
      retErr = 1
      serialEventExit(ser)
      return retErr

#Testphase 4 - Server Test
    cmd = serialESPCmd.serverTest
    rpiSendCmd(ser, cmd)
    if rpiReceiveCorrect(ser) == serialESPCmd.receiveCorrect:
      print 'Receive correct'
      isErr, val = rpiReceiveData(ser, cmd)
      if not isErr and val[0]==serialESPCmd.operationSuccessful:
        print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
      else:
        print 'Timeout'
        retErr = 1
        serialEventExit(ser)
        return retErr
    else:
      print 'Error'
      retErr = 1
      serialEventExit(ser)
      return retErr

#Testphase 5 - SoftAP Test
    cmd = serialESPCmd.softAPTest
    rpiSendCmd(ser, cmd)
    if rpiReceiveCorrect(ser) == serialESPCmd.receiveCorrect:
      print 'Receive correct'
      isErr, val = rpiReceiveData(ser, cmd)
      if not isErr and val[0]==serialESPCmd.operationSuccessful:
        print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
      else:
        print 'Timeout'
        retErr = 1
        serialEventExit(ser)
        return retErr
    else:
      print 'Error'
      retErr = 1
      serialEventExit(ser)
      return retErr

#Testphase 6 - Hardware Test
    cmd = serialESPCmd.checkBoardIO
    rpiSendCmd(ser, cmd)
    if rpiReceiveCorrect(ser) == serialESPCmd.receiveCorrect:
      print 'Receive correct'
      isErr, val = rpiReceiveData(ser, cmd)
      if not isErr and val[0]==serialESPCmd.operationSuccessful:
        print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
      else:
        print 'Timeout'
        retErr = 1
        serialEventExit(ser)
        return retErr
    else:
      print 'Error'
      retErr = 1
      serialEventExit(ser)
      return retErr

#Testphase 7 - ReadyToQuit
    cmd = serialESPCmd.readyToQuit
    rpiSendCmd(ser, cmd)
    if rpiReceiveCorrect(ser) == serialESPCmd.receiveCorrect:
      print 'Receive correct'
      isErr, val = rpiReceiveData(ser, cmd)
      if not isErr and val[0]==serialESPCmd.operationSuccessful:
        print 'Data received: ', ' '.join(hex(val[x]) for x in range(0, len(val)))
      else:
        print 'Timeout'
        retErr = 1
        serialEventExit(ser)
        return retErr
    else:
      print 'Error'
      retErr = 1
      serialEventExit(ser)
      return retErr

#End
    serialEventExit(ser)
    return retErr

  except:
    raise
    if (ser.isOpen()==True):
     ser.close()
    return 1

def serialEventExit(ser):
  print 'closing port: ', ser.port
  ser.close()

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
     _header, _flag, _packet_len, _response, _checksum = [ord(inByteString[x]) for x in range(0, 5)]
     #_header = ord(inByteString[0])
     #_flag = ord(inByteString[1])
     #_packet_len = ord(inByteString[2])
     #_response = ord(inByteString[3])
     #_checksum = ord(inByteString[4])
     _cal_cs = (170 + _flag + _packet_len + _response) % 256
     if _header == 0x5c and _flag == 0x30 and _packet_len == 1 and _checksum == _cal_cs: 
       return _response
  return 0

def rpiReceiveData(ser, cmd, timeout=3):
  ser.timeout = timeout
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
         return (1, 0)
       _data = inByteString[1:_packet_len]
       #print 'Date received: ' ,_data.encode('hex')
       _checksum = ord(inByteString[_packet_len])
       #print _checksum
       _convert_data = array.array('i', (ord(_data[x]) for x in range(0, len(_data))))
       _cal_checksum = (170 + _flag + _packet_len + _cmd + sum(_convert_data)) % 256
       #print _cal_checksum
       if _checksum != _cal_checksum:
         print 'checksum error'
         return (1, 0)
       return (0, _convert_data)
  return (1, 0)
  
def main(args):
  return startSerial(args[1])

if __name__ =='__main__':
  sys.exit(main(sys.argv))
