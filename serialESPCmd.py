#!/usr/bin/python
#this file contains constant variables for serialESP command

#Command
sync = 0x40
readyToQuit = 0x41
retrieveInfo = 0x42
checkWiFiConnection = 0x43
clientTest = 0x44
serverTest = 0x45
softAPTest = 0x46
checkBoardIO = 0x47

#Response code
receiveCorrect = 0x01
receiveError = 0x02
operationSuccessful = 0x31
timeOut = 0x32