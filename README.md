<h1 align = "center">REMDx</h1>

REMDx is a board that is able to measure **0-35V DC voltage** with a resolution of 1mV, **0-8A DC current** with a resolution of 250uA (which can be changed by replacing the shunt resistor), **burden voltage**, and **power**. Furthermore, all measurements can be performed over two channels.
The REMDx (Realtime Efficiency Measurement Unit x) was originally intended to measure the efficiency of Switch Mode Power Supplies / Linear Regulator hence the name REMDx but later evolved into a datalogging device. See the [schematic](https://github.com/quractech/remdx/blob/main/remd_scheamtic.pdf) of REMDx ([Github repo](https://github.com/quractech/remdx)).
<br>
Below you can see the REMDx PCB board.
![REMDx board](board_v3.jpg)
The system block diagram of the REMDx is shown below.
![Block diagram](concept_white.png)

## Capability

The REMDx is capable of measuring:

- 2 channel voltage measurement 0-35V with resolution of approximately 1mV
- 2 channel current measurement 0-8A with resolution of approximately 250uA, coms with 10mOhm shunt resistor.
- 2 channel power measurement
- 2 channel burden voltage measurement

### J9 CONNECTOR PINOUT

- 4 Digital input pins max. 3.3V (INTx - interrupt pins)
- 2 Analog input pins max 3.3V (MCU ADC with 12bit)
- 2 Digital output pins with selectable output voltage levels (3.3V, 5V, and 12V)
- 2 PWM pins with selectable output voltage levels (3.3V, 5V, and 12V)

### SD-card

Micro-SDcard slot for saving stream data (stream data is serial data from USB).
The current firmware does not have this functionality, however, it will be added soon.

### OLED Display

The OLED display (ssd1306) displays the bus voltages (Vbusx), currents (Isensx), burden voltages (Vburdx), and powers (Powerx) for both channels. On the first row of the OLED display, the ambient temperature and efficiency are also shown.
The OLED display data layout is shown in the figure below.

<p align="center">
<img src="oled_disp_data_layout_black.png" width="40%">
</p>

### Real-time plotting with python

Data stream capability using USB for datalogging and visualization with Python (see scripting folder)

## Getting Started

### **Upgrading REMDx to the most recent firmware**

1. Download and install [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)<br>
   <img src="stm32cubeprogrammer.png" width="75%">
2. Connect REMDx to your PC with a USB cable that is capable of data transfer.
3. On REMDx press and hold the BOOT button, then press the RESET button and let go of both buttons. This will put REMDx in programming mode.
4. Open STM32CubeProgrammer select **USB** (Blue dropdown menu on the right column), then click the **refresh** icon until "USB1" shows up on port (Serial number 2081325A4146), then click **connect**.<br> <img src="./connectstm32cubeprogrammer.png" width="75%">
5. Download [FW.elf](https://github.com/quractech/remdx/blob/main/FW/FW.elf) file to your PC. Go back to STM32CubeProgrammer press "open file" then press download. Now REMDx should be programmed or updated to the newest firmware.

### **Get REMDx to Communicate**

7. Download and install [Realterm](https://realterm.sourceforge.io/index.html#downloads_Download) or similar terminal program.<br>
   <img src="https://realterm.sourceforge.io/realterm1.png" width="75%">
8. Select the COMPORT, find it under Device Manager in Windows <br> <img src="comport.png" height="75%" width="75%">
9. Select the BAUD RATE 115200 and connect, now you should see a continuous data stream as shown in the picture below.<br>
   <img src="realterm_connect.png" height="75%" width="75%">

### **Register Map**

REMDx can be configured using [register map](https://quractech.github.io/remdx_registermap/), and the configuration can be saved in EEPROM (default configuration can be restored).

**COMMUNICATION PROTOCOL**<br>
The REMDx should be written as follows:<br>

1. A check command **"\* L #"** of 5 bytes ("" are not part of the command) is sent to the device as **numbers**. Where L is the length of the command sent next (L is always 5).<br>
2. To write to or read from the device, a second command of 5 bytes in **ASCII** is sent. <br>

<img src="./protocol.png">
The check command (step 1) must first be successfully received before the second command (step 2) may be sent.<br>
The second command is referred to as "the command" from this point on.<br>

- Byte 1: Indicate the command to read or write
- Bytes 2 and 3: Specify the register you are writing to or reading from (32 registers are available for read or write operation).<br>
- Bytes 4 and 5: Specify the data you are writing to the device. When reading from a register, ignore these two bytes and set them to 0.

To indicate register data to and from the device, hexadecimal numbers are utilized.
<br><br>
Use the letter **"w"** as the initial character of the command you are sending to the device if you want to **write** to a specific register.<br>

### **Write and Read Example**

Connect REMDx to your PC and open RealTerm.<br>
To stop stream data from REMDx, set the **_meas_mode_** bit to 0, see [register map](https://quractech.github.io/remdx_registermap/)<br>

Stop stream data.

```bash
w0304
```

Writing 0x07 to register 2.

```bash
w0207
```

w - to specify a write command<br>
02 - register number 2 as hex value<br>
07 - data written to register 2 as a hex value.<br>
Relating to the register map:<br>
**_set_led1_=1**<br>
**_set_led2_=1**<br>
As a result of this command, the onboard LEDs should turn on
<br>

Use the letter **"r"** as the initial character of the command you are sending to the device if you want to **read** from a specific register.
<br>
Reading the content of register 2.

```bash
r0200
```

r - to specify a read command<br>
02 - register number 2 as hex value<br>
00 - Not important when reading a register.<br>
Outcome of RealTerm<br>
<img src="registermap_example.png" width="75%">

### **Python scripting**

Python scripting also allows for the reconfiguration of REMDx and live data visualization.
<br>
Here is some sample code to turn on led1 and led2.

```python
from datacontrol import dataCTRLMaster
from plotcontrol import plotMaster
remd = dataCTRLMaster()

# LED Example
print(f"set_led1 = {remd.set_led1}")
remd.set_led1 = 1
print(f"set_led1 = {remd.set_led1}")
print(f"set_led2 = {remd.set_led2}")
remd.set_led2 = 1
print(f"set_led2 = {remd.set_led2}")
```

Example code for live plot of bus voltage of channel1.

```python
from datacontrol import dataCTRLMaster
from plotcontrol import plotMaster
remd = dataCTRLMaster()

# Example - Live plot of the bus voltage of channel 1
plotter.plotData(1)
```

<img src="remdx.gif">
<img src="remdx_liveplot.gif">
