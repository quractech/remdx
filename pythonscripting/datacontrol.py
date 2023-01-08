from serialcontrol import serialCTRLMaster
import matplotlib.pyplot as plt
import concurrent.futures
import numpy as np
import time
import timeit
import threading


class dataCTRLMaster():
    def __init__(self, fileName=False):
        self.fileName = fileName
        self.uart = serialCTRLMaster()
        # self.flushUART()

        # self.uart.loadFile(fileName)
        # self.uart.Write("w0200")

    def load(self, file):
        self.uart.loadFile(file)

    def flushUART(self):
        self.uart.Write("r0000")
        self.readReg(0)

    def readReg(self, register) -> str:
        # print("---------- readReg ----------")
        counter = 0
        max_iterations = 5

        if(register <= 15):
            reg = "r0"+hex(register)[2:]+"00"
        else:
            reg = "r"+hex(register)[2:]+"00"
        data = "000000"
        try:
            self.uart.Write(reg)
            # FIXME - Getting stug here
            # and counter < max_iterations:
            while(data[0:3] != "#R#" or int(str("0x"+data[3:5]), 0) != register):
                self.uart.Write(reg)
                data = self.uart.read()
                counter += 1
            # if counter >= max_iterations:
            #     # Return an error message if the loop exceeded the maximum number of iterations
            #     # return "Error: maximum number of iterations exceeded"
            #     self.uart.Write(reg)
            #     data = self.uart.read()
            # else:
            return str("0x"+str(data[3:]))
            # return str("0x"+str(data[3:]))
        except Exception as error:
            # pass
            print(f"ERROR(readReg): {error}")
            # self.readReg(register)
            # return "0x0000"

    def restartDevice(self, restart):
        reg = 18
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        TxData = (hex(int(RxData, 0) |
                      restart)[2:]) if restart else (hex(int(RxData, 0) & (1022 | restart))[2:])
        TxData = "w" + str(TxData)
        # self.uart.Write(TxData)
        while(ack != "ACK"):
            self.uart.Write(TxData)
            ack = self.uart.read()

    @property
    def startStream(self) -> int:
        reg = 3
        RxData = self.readReg(reg)
        return ((int(RxData, 0) & 2) >> 1)

    @startStream.setter
    def startStream(self, enable):
        # print("---------- startStream ----------")
        # self.startStream
        reg = 3
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        if(((int(RxData, 0) & 2) >> 1) != 1):
            TxData = (hex(int(RxData, 16) | (enable << 1))[2:])
            TxData = "w0"+TxData

            while(ack != "ACK"):
                self.uart.Write(TxData)
                ack = self.uart.read()

    @property
    def dispEnable(self) -> int:
        reg = 3
        RxData = self.readReg(reg)
        return ((int(RxData, 0) & 4) >> 2)

    @dispEnable.setter
    def dispEnable(self, enable):
        self.startStream
        reg = 3
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        if(((int(RxData, 0) & 4) >> 2) != 1):
            TxData = (hex(int(RxData, 16) | (enable << 2))[2:])
            TxData = "w0"+TxData

            while(ack != "ACK"):
                self.uart.Write(TxData)
                ack = self.uart.read()

    @property
    def id0(self) -> str:
        RxData = self.readReg(0)
        return "v"+str((int(RxData, 0) & 255)/10)

    @property
    def id1(self) -> str:
        RxData = self.readReg(1)
        return "v"+str((int(RxData, 0) & 255)/10)

    @property
    def set_led1(self) -> int:
        RxData = self.readReg(2)
        return int(RxData, 0) & 1

    @set_led1.setter
    def set_led1(self, enableLED):
        # self.LED1BlinkEnable = 0
        trial = 0
        max_trial = 5
        reg = 2
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        if(enableLED):
            TxData = hex(int(RxData, 0) | enableLED)[2:]
        else:
            TxData = hex(int(RxData, 0) & (1022 | enableLED))[2:]
        # TxData = (hex(int(RxData, 0) |
        #               enableLED)[2:]) if enableLED else (hex(int(RxData, 0) & (1022 | enableLED))[2:])
        TxData = "w0" + str(TxData)
        # self.uart.Write(TxData)
        while(ack != "ACK" and trial < max_trial):
            self.uart.Write(TxData)
            ack = self.uart.read()
            # print(ack)
            trial += 1
        if(trial >= max_trial):
            self.uart.Write(TxData)

        # print("Done with LED1")

    @property
    def set_led2(self) -> int:
        RxData = self.readReg(2)
        return (int(RxData, 0) >> 1) & 1

    @set_led2.setter
    def set_led2(self, enableLED):
        # self.LED2BlinkEnable = 0
        trial = 0
        max_trial = 10
        reg = 2
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        # TxData = (hex(int(RxData, 0) |
        #               enableLED << 1)[2:]) if enableLED else (hex(int(RxData, 0) & (1021 | enableLED << 1))[2:])
        if enableLED:
            TxData = hex(int(RxData, 0) | (enableLED << 1))[2:]
        else:
            TxData = hex(int(RxData, 0) & (1021 | enableLED << 1))[2:]

        TxData = "w0" + str(TxData)
        while(ack != "ACK" and trial < max_trial):
            self.uart.Write(TxData)
            ack = self.uart.read()
            trial += 1
        if(trial >= max_trial):
            self.uart.Write(TxData)

        print("Done with LED2")

    @property
    def set_dig_out0(self) -> int:
        RxData = self.readReg(2)
        return (int(RxData, 0) >> 2) & 1

    @set_dig_out0.setter
    def set_dig_out0(self, enable):
        reg = 2
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        TxData = (hex(int(RxData, 0) |
                      enable << 2)[2:]) if enable else (hex(int(RxData, 0) & (1019 | enable << 2))[2:])
        TxData = "w0" + str(TxData)
        while(ack != "ACK"):
            self.uart.Write(TxData)
            ack = self.uart.read()

    @property
    def set_dig_out1(self) -> int:
        RxData = self.readReg(2)
        return (int(RxData, 0) >> 3) & 1

    @set_dig_out1.setter
    def set_dig_out1(self, enable):
        reg = 2
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        TxData = (hex(int(RxData, 0) |
                      enable << 3)[2:]) if enable else (hex(int(RxData, 0) & (1015 | enable << 3))[2:])
        TxData = "w0" + str(TxData)
        while(ack != "ACK"):
            self.uart.Write(TxData)
            ack = self.uart.read()

    @property
    def get_int_in0(self) -> int:
        RxData = self.readReg(2)
        return (int(RxData, 0) >> 4) & 1

    @property
    def get_int_in1(self):
        RxData = self.readReg(2)
        return (int(RxData, 0) >> 5) & 1

    @property
    def auto_off_dis(self) -> int:
        RxData = self.readReg(3)
        return (int(RxData, 0)) & 1

    @auto_off_dis.setter
    def auto_off_dis(self, enable):
        reg = 3
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        TxData = (hex(int(RxData, 0) |
                      enable)[2:]) if enable else (hex(int(RxData, 0) & (2046 | enable))[2:])
        TxData = "w0" + str(TxData)
        while(ack != "ACK"):
            self.uart.Write(TxData)
            ack = self.uart.read()

    def setLED1Blink(self, enableBlink, blinkRate):
        trial = 0
        max_trial = 10
        ack = None
        blinkRate = blinkRate/10
        if(blinkRate > 127):
            blinkRate = 127
            print("The maximum LED blinkrate is 1270 ms")
            print("LED blinkrate set to 1270 ms")
        startData = int("0x1500", 0)
        TxData = hex(startData | (int(blinkRate) << 1) | enableBlink)[2:]
        TxData = "w"+TxData

        while(ack != "ACK" and trial < max_trial):
            self.uart.Write(TxData)
            ack = self.uart.read()
            # print(ack)
            trial += 1
        if(trial >= max_trial):
            self.uart.Write(TxData)

        # print("Done with LED1 blink")

    @property
    def LED1BlinkEnable(self) -> int:
        RxData = self.readReg(21)
        return int(RxData, 0) & 1

    @LED1BlinkEnable.setter
    def LED1BlinkEnable(self, enableBlink):
        RxData = self.readReg(21)
        RxData = ((int(RxData, 0) & 255) >> 1)*10
        # print(RxData)
        self.setLED1Blink(enableBlink, RxData)

    @property
    def LED1BlinkRate(self) -> int:
        RxData = self.readReg(21)
        return ((int(RxData, 0) >> 1) & 127)*10

    @LED1BlinkRate.setter
    def LED1BlinkRate(self, blinkRate):
        RxData = self.readReg(21)
        RxData = int(RxData, 0) & 1
        self.setLED1Blink(RxData, blinkRate)

    def setLED2Blink(self, enableBlink, blinkRate):
        trial = 0
        max_trial = 10
        ack = None
        blinkRate = blinkRate/10
        if(blinkRate > 127):
            blinkRate = 127
            print("The maximum LED blinkrate is 1270 ms")
            print("LED blinkrate set to 1270 ms")
        startData = int("0x1600", 0)
        TxData = hex(startData | (int(blinkRate) << 1) | enableBlink)[2:]
        TxData = "w"+TxData

        while(ack != "ACK" and trial < max_trial):
            self.uart.Write(TxData)
            ack = self.uart.read()
            # print(ack)
            trial += 1
        if(trial >= max_trial):
            self.uart.Write(TxData)

        # print("Done with LED2 blink")

    @property
    def LED2BlinkEnable(self) -> int:
        RxData = self.readReg(22)
        return int(RxData, 0) & 1

    @LED2BlinkEnable.setter
    def LED2BlinkEnable(self, enableBlink):
        RxData = self.readReg(22)
        RxData = ((int(RxData, 0) & 255) >> 1)*10
        # print(RxData)
        self.setLED2Blink(enableBlink, RxData)

    @property
    def LED2BlinkRate(self) -> int:
        RxData = self.readReg(22)
        return ((int(RxData, 0) >> 1) & 127)*10

    @LED2BlinkRate.setter
    def LED2BlinkRate(self, blinkRate):
        RxData = self.readReg(22)
        RxData = int(RxData, 0) & 1
        self.setLED2Blink(RxData, blinkRate)

    @property
    def PWMFreq(self) -> int:
        freq1 = (int(self.readReg(14), 0) & 255)
        freq2 = (int(self.readReg(15), 0) & 255) << 8
        return (freq2+freq1)

    @PWMFreq.setter
    def PWMFreq(self, freq):
        regBuffer = [14, 15]
        ack = "00000"
        freq1 = (freq) & 255
        freq2 = (freq >> 8) & 255
        freqBuffer = [freq1, freq2]

        for i in range(0, 2):
            ack = "00000"
            TxData = hex(regBuffer[i] << 8 | freqBuffer[i])[2:]
            if(len(TxData) < 4):
                TxData = "w0"+TxData
            else:
                TxData = "w"+TxData
            while(ack != "ACK"):
                self.uart.Write(TxData)
                ack = self.uart.read()

    @property
    def PWM0Enable(self) -> int:
        return ((int(self.readReg(2), 0) >> 6) & 1)

    @ PWM0Enable.setter
    def PWM0Enable(self, PWMEnable):
        reg = 2
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        TxData = (hex(int(RxData, 0) |
                      PWMEnable << 6)[2:]) if PWMEnable else (hex(int(RxData, 0) & (0b000_0011_1011_1111 | PWMEnable << 6))[2:])
        TxData = "w0" + str(TxData)
        while(ack != "ACK"):
            self.uart.Write(TxData)
            ack = self.uart.read()

    @ property
    def PWM1Enable(self) -> int:
        return ((int(self.readReg(2), 0) >> 7) & 1)

    @ PWM1Enable.setter
    def PWM1Enable(self, PWMEnable):
        reg = 2
        ack = None
        TxData = ""
        RxData = self.readReg(reg)
        TxData = (hex(int(RxData, 0) |
                      PWMEnable << 7)[2:]) if PWMEnable else (hex(int(RxData, 0) & (0b000_0011_0111_1111 | PWMEnable << 7))[2:])
        TxData = "w0" + str(TxData)
        while(ack != "ACK"):
            self.uart.Write(TxData)
            ack = self.uart.read()

    @ property
    def PWM0Duty(self) -> int:
        return (int(self.readReg(12), 0) & 255)

    @ PWM0Duty.setter
    def PWM0Duty(self, duty):
        ack = "0000"
        if(duty > 100):
            duty = 100
        startData = int("0x0C00", 0)
        TxData = hex(startData | (int(duty)))[2:]
        TxData = "w0"+TxData

        while(ack != "ACK"):
            self.uart.Write(TxData)
            ack = self.uart.read()

    @ property
    def PWM1Duty(self) -> int:
        return (int(self.readReg(13), 0) & 255)

    @ PWM1Duty.setter
    def PWM1Duty(self, duty):
        ack = "0000"
        if(duty > 100):
            duty = 100
        startData = int("0x0D00", 0)
        TxData = hex(startData | (int(duty)))[2:]
        TxData = "w0"+TxData

        while(ack != "ACK"):
            self.uart.Write(TxData)
            ack = self.uart.read()

    def getBusVoltage(self, channel: int) -> float:
        stream_data = ""
        channel_map = {1: 4, 2: 8}

        # Check if the given channel is valid
        if channel not in channel_map:
            raise ValueError(f"Invalid channel: {channel}")

        # Wait until the UART stream has the expected format
        raw_stream_data = self.uart.getRawData()
        while not raw_stream_data.startswith("#D#") or len(raw_stream_data) < 80:
            raw_stream_data = self.uart.getRawData()

        # Split the stream data and get the bus voltage for the given channel
        stream_data = raw_stream_data.split(",")
        bus_voltage = float(stream_data[channel_map[channel]])
        return bus_voltage

    def getCurrent(self, channel) -> float:
        streamData = "000000"
        self.startStream = 1

        while(len(str(streamData)) < 80):
            streamData = self.uart.read()

        streamData = streamData.split(',')
        if (streamData[0][:3] == "#D#"):
            if(channel == 1):
                streamData = float(streamData[5])
                streamData = float(f"{streamData:.3f}")
                return streamData
            elif(channel == 2):
                streamData = float(streamData[9])
                return float(f"{streamData:.6f}")
            else:
                # return None
                self.getCurrent(channel)
        else:
            # return None
            self.getCurrent(channel)

    def getPower(self, channel) -> float:
        streamData = "000000"
        self.startStream = 1

        while(len(str(streamData)) < 80):
            streamData = self.uart.read()

        streamData = streamData.split(',')
        if (streamData[0][:3] == "#D#"):
            if(channel == 1):
                streamData = float(streamData[6])
                return float(f"{streamData:.3f}")
            elif(channel == 2):
                streamData = float(streamData[10])
                return float(f"{streamData:.3f}")
            else:
                # return None
                self.getPower(channel)
        else:
            # return None
            self.getPower(channel)

    def getBurdenVoltage(self, channel) -> float:
        streamData = "000000"
        self.startStream = 1

        while((len(str(streamData)) < 80)):
            streamData = self.uart.read()

        streamData = streamData.split(',')
        if(streamData[0][:3] == "#D#"):
            if (channel == 1):
                streamData = float(streamData[3])
                return float(f"{streamData:.6f}")
            elif(channel == 2):
                streamData = float(streamData[7])
                return float(f"{streamData:.6f}")
            else:
                # return None
                self.getBurdenVoltage(channel)
        else:
            # return None
            self.getBurdenVoltage(channel)

    def getEfficiency(self) -> float:
        if(self.getPower(2) > 0):
            efficiency = float(self.getPower(2)/self.getPower(1))
            return float(f"{efficiency:.3f}")
        else:
            # return None
            self.getEfficiency()

    def getTemp(self) -> float:
        streamData = "000000"
        self.startStream = 1

        while((streamData[:3] != "#D#") and (len(str(streamData)) < 80)):
            streamData = self.uart.read()

        streamData = streamData.split(',')
        if(len(streamData) > 5):
            streamData = streamData[0]
            streamData = float(streamData[3:])
            return streamData
        else:
            # return None
            self.getTemp()

    def getMCUTemp(self) -> float:
        streamData = "000000"
        self.startStream = 1

        while((streamData[:3] != "#D#") and (len(str(streamData)) < 80)):
            streamData = self.uart.read()

        streamData = streamData.split(',')
        if(len(streamData) > 11):
            streamData = streamData[1]
            streamData = float(streamData)
            return streamData
        else:
            # return None
            self.getMCUTemp()
