from base64 import encode
from socket import timeout
import time
import serial
import serial.tools.list_ports
import threading
import concurrent.futures
import timeit


class serialCTRLMaster():

    def __init__(self, comPort=None, baudRate=115200, fileName=None):
        self.ser = serial
        self.uart = None
        self.baudRate = baudRate
        self.comPort = comPort
        self.fileContent = []
        self.registerData = ""
        self.streamData = ""
        self.manual = False
        self.chk = bytearray([42, 5, 35])

        if (self.baudRate != 115200):
            self.baudRate = baudRate

        if (self.comPort == None):
            self.findComPort()
        # else:
        #     self.comPort = input("Enter Comport: ")
        self.serialOpen()
        # self.restartDevice()

    def findComPort(self):
        for com in self.ser.tools.list_ports.comports():
            if ("STMicroelectronics Virtual COM Port" in com[1]
                    or "USB-SERIAL CH340" in com[1]):
                self.comPort = com[0]
        if (not self.comPort):
            self.manual = True
            self.findManualPort(self.manual)

    def findManualPort(self, manual):
        if (self.manual):
            self.manual = False
            self.comPort = input("Enter Comport: ")

    def serialOpen(self):
        try:
            self.ser.is_open
        except:
            self.ser.timeout = 2
            self.uart = self.ser.Serial(self.comPort, self.baudRate,
                                        self.ser.EIGHTBITS,
                                        self.ser.PARITY_NONE,
                                        self.ser.STOPBITS_ONE)
        try:
            if self.ser.is_open:
                self.ser.status = True
            else:
                self.ser = serial.Serial()
                self.ser.baudrate = self.baudRate
                self.ser.port = self.comPort
                self.ser.timeout = 2
                self.ser.status = True
        except:
            self.ser.status = False

    def serialClose(self):
        try:
            self.ser.is_open
            self.ser.close()
            self.ser.status = False
        except:
            self.ser.status = False

    def read(self) -> str:
        raw_data = self.uart.readline()

        if not raw_data:
            print("ERROR: Empty raw data")
            return ""
        try:
            decoded_data = ""
            decoded_data = str(raw_data[0:len(raw_data)].decode("utf-8"))
            return decoded_data.strip()
        except Exception as error:
            print(f"ERROR(decode): {error}")

    def readTillACK(self, ack):
        with concurrent.futures.ThreadPoolExecutor() as executor:
            uartReading = executor.submit(self.read)
            data = uartReading.result()
            if (data[0:3] == ack):
                return data

    def serialRead(self):
        with concurrent.futures.ThreadPoolExecutor() as executor:
            uartReading = executor.submit(self.read)
            return uartReading.result()

    def Write(self, cmd):
        try:
            self.uart.write(self.chk)
            self.uart.write(cmd.encode('utf-8'))
            time.sleep(0.1)
        except Exception as error:
            print(f'ERROR(Write): {error}')
            self.serialOpen()

    def serialWrite(self, cmd):
        with concurrent.futures.ThreadPoolExecutor() as executor:
            uartReading = executor.submit(self.Write, cmd)
            return uartReading.result()

    def readFile(self, fileName):
        self.fileName = fileName
        with open(self.fileName, 'r') as file:
            for line in file:
                if (line[0] == "w" or line[0] == "r"):
                    line = line[0:5]
                    self.fileContent.append(line)

    def loadFile(self, fileName):
        self.readFile(fileName)
        for cmd in self.fileContent:
            self.Write(cmd)

    def restartDevice(self):
        self.Write("w1201")

    def getReg(self, register: int) -> str:
        counter = 0
        max_iterations = 5
        if (register <= 15):
            reg = "r0" + hex(register)[2:] + "00"
        else:
            reg = "r" + hex(register)[2:] + "00"

        self.Write(reg)

        regData = self.read()
        while (regData[0:3] != "#R#"
               or int(str("0x" + regData[3:5]), 0) != register):
            regData = self.read()
            self.Write(reg)
        return regData

    def setReg(self):
        pass

    def getRawData(self) -> str:
        max_trial = 5
        trial = 0
        rawData = "00000"
        while (rawData[0:3] != '#D#'):  # or not rawData):
            rawData = self.read()
            if (trial >= max_trial):
                break
            trial += 1

        return rawData
