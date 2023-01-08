import matplotlib.pyplot as plt
import time
from matplotlib.animation import FuncAnimation
import sys
import os
import datetime


class plotMaster():
    def __init__(self, dataCTRLMaster, plotRefeshTime=100, captureSize=120):
        self.xData = []
        self.yData = []
        self.remd = dataCTRLMaster
        # self.remd = remd
        self.start_time = time.time()
        self.t0 = self.start_time
        self.t1 = self.t0
        self.elapse_time = []
        self.result = 0.0
        # The maximum time captured in the plot in seconds.
        self.captureSize = captureSize
        self.plotRefeshTime = plotRefeshTime    # The plot refresh time in seconds
        self.SAMPLE_SIZE = 1e3*self.captureSize/(self.plotRefeshTime+100)
        self.time_now = datetime.datetime.now()
        self.today = datetime.date.today()

    def updatePlot(self, i, ch):
        try:
            t = time.time()-self.start_time
            self.result = self.remd.getBusVoltage(ch)
            # self.result = self.remd.getCurrent(ch)
            # self.result = self.remd.getPower(ch)
            # self.result = self.remd.getBurdenVoltage(ch)*1e3
            # print(self.result)
            # self.t0 = self.t1

            if self.result != None:
                self.yData.append(self.result)
                self.xData.append(t)

            if len(self.yData) > self.SAMPLE_SIZE-1:
                self.yData.pop(0)
                self.xData.pop(0)
            # print(f"time[0]\t\t= {self.xData[0]:.3f}")
            # print(f"time[-1]\t= {self.xData[-1]:.3f}")
            # self.t1 = time.time()
            # self.elapse_time.append((self.t1-self.t0))
            # self.t1 = self.elapse_time[-1]
            plt.cla()
            # plt.clf()
            plt.plot(self.xData, self.yData)
            # plt.plot(self.xData, self.elapse_time)
            plt.scatter(self.xData[-1], self.yData[-1])
            plt.scatter(self.xData[-1], self.yData[-1])
            #             label="{:.2f}V".format(self.yData[-1]))
            # plt.legend(loc='upper left')
            plt.xlabel("time (s)")
            plt.ylabel("Bus Voltage (V)")
            plt.title(f"Ch{ch} reading {self.result:.2f} V")
            # plt.title(f"Ch{ch} reading {(self.t1-self.t0):.2f} V")
            plt.xlim(left=max(0, t-50), right=t+50)
            plt.xlim(self.xData[0], max(self.xData)*1.01)
            if(min(self.yData) > 0):
                plt.ylim(min(self.yData)*.95, max(self.yData)*1.05)
            plt.tight_layout()
            plt.grid()
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            # print(exc_type, fname, exc_tb.tb_lineno)
            # with open("log.txt", "a") as f:
            #     f.write(
            #         f"{self.time_now.strftime('%H:%M:%S')} - {self.today.strftime('%d/%m/%Y')}\tERROR\t{exc_type}, {fname}, {exc_tb.tb_lineno}\n")

    def plotData(self, ch):
        self.t0 = time.time()
        ani = FuncAnimation(plt.gcf(), self.updatePlot,
                            repeat=False, fargs=(ch,), interval=self.plotRefeshTime)
        plt.show()
