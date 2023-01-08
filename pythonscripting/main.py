from matplotlib.animation import FuncAnimation
import matplotlib.pyplot as plt
import time
from datacontrol import dataCTRLMaster
from plotcontrol import plotMaster
from datacontrol import dataCTRLMaster
from plotcontrol import plotMaster
remd = dataCTRLMaster()
plotter = plotMaster(remd)
if __name__ == "__main__":
    # Load configuration from a file
    # file = "pwm0.rmd"
    # remd = dataCTRLMaster()
    # remd.load(file)

    # LED Example
    print(remd.id0)
    print(f"set_led1 = {remd.set_led1}")
    remd.set_led1 = 1
    print(f"set_led1 = {remd.set_led1}")
    print(f"set_led2 = {remd.set_led2}")
    remd.set_led2 = 1
    print(f"set_led2 = {remd.set_led2}")
    # remd.auto_off_dis = 1
    # remd.startStream = 0
    # print(remd.startStream)
    # print("LED1")
    # remd.LED1BlinkEnable = 1
    # remd.LED1BlinkRate = 200

    # print("LED2")
    # remd.LED2BlinkEnable = 1
    # remd.LED2BlinkRate = 1000

    # print(f"LED1Enable = {remd.LED1Enable}")
    # print(f"LED1BlinkEnable = {remd.LED1BlinkEnable}")
    # print(f"LED1BlinkRate = {remd.LED1BlinkRate}")

    # print(f"LED2Enable = {remd.LED2Enable}")
    # print(f"LED2BlinkEnable = {remd.LED2BlinkEnable}")
    # print(f"LED2BlinkRate = {remd.LED2BlinkRate}")

    #  PWM Example
    # remd.PWMFreq = 500
    # print(remd.PWMFreq)  # = 1000

    # remd.PWM0Duty = 25
    # remd.PWM1Duty = 75
    # print(remd.PWM0Duty)
    # print(remd.PWM1Duty)

    # remd.PWM0Enable = 1
    # print(remd.PWM0Enable)
    # remd.PWM1Enable = 1
    # print(remd.PWM1Enable)

    # print("Print(Vbus)")
    # GET DATA STREAM Example
    # print(remd.getBusVoltage(1)+" V")
    # remd.getBusVoltage(1)
    # remd.getBurdenVoltage(1)
    # remd.getCurrent(1)
    # remd.getPower(1)

    # remd.getBusVoltage(2)
    # remd.getBurdenVoltage(2)
    # remd.getCurrent(2)
    # remd.getPower(2)

    # print(f"{remd.getEfficiency()*100:.1f} %")

    # GET AMB. TEMPERATURE SENSOR READINGS
    # print(remd.getTemp())

    # GET MCU TEMPERATURE
    # print(remd.getMCUTemp())
    # remd.startStream = 1
    # remd.dispEnable = 0
    # print(remd.dispEnable)
    # remd.dispEnable = 1
    # print(remd.dispEnable)

    # while(1):
    #     if(remd.getBusVoltage(1) > 10.0):
    #         remd.LED1BlinkEnable = 1
    #         remd.LED2BlinkEnable = 1
    #         remd.LED1BlinkRate = 100
    #         remd.LED2BlinkRate = 100
    #     else:
    #         remd.LED1BlinkEnable = 0
    #         remd.LED2BlinkEnable = 0

    # TEST READINGS
    # plotter.plotData(1)
