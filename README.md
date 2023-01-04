The REMDx a board that is able to measure 0-35V DC voltage with a resolution of 1mV, 0-8A DC current with a resolution of 250uA (which can be modified), burden voltage, and power. Furthermore, all measurements can be performed over two channels.
The REMDx (Realtime Efficiency Measurement Unit x) was originally intended to measure effeciency of Switch Mode Power Supplies / Linear Regulator hence the name REMDx, but later evolved to a datalogging device.

Concept diagram of REMDx is shown below.
![Block diagram](concept_white.png)
The REMDx PCB
![REMDx board](board_v3.jpg)

![REMDx board](remd_board_top.png)

## Capability
The REMDx is capable of measuring:
- 2 channel voltage measurement 0-35V with resolution of approximately 1mV
- 2 channel current measurement 0-8A  with resolution of approximately 250uA, coms with 10mOhm shunt resistor.
- 2 channel power measurent
- 2 channel burden voltage measurent
### PINOUT
J9 connect
- 4 Digital input pins max. 3.3V (INTx - interrupt pins)
- 2 Analog input pins max 3.3V (MCU ADC with 12bit)
- 2 Digital output pins with selectable output voltage level (3.3V, 5V, and 12V)
- 2 PWM pins with selectable output voltage level (3.3V, 5V, and 12V)

### SD-card
Micro-SDcard slot for saving stream data (stream data is serial data from USB).
### Real-time plotting with python
Data stream capability using USB for datalogging and visualization with python (see scripting folder)
### Register Map
REMDx can be configured using [register map](https://quractech.github.io/remdx_registermap/), the configuration can be saved in EEPROM (default configuration can be restored).

## Getting Started
1. Download and install [Realterm](https://realterm.sourceforge.io/index.html#downloads_Download) or similar terminal program.
![RealTerm](https://realterm.sourceforge.io/realterm1.png)
<img src="https://realterm.sourceforge.io/realterm1.png" width="150" height="280">

2. Select the COMPORT, find it under Device Manager in windows
    ![Comport](comport.PNG)
3. Select the BAUD RATE 115200 and connect, now you should see a contineous data stream as show in the picture below.
![RealTerm Connect](realterm_connect.png)




