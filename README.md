# REMDx
The REMDx can measure 0-35V DC voltages with a resolution of 1mV, 0-8A DC currents with a resolution of 250uA (which can be modified), burden voltages, and power. Furthermore, all measurements can be performed over two channels.
The REMDx (Realtime Efficiency Measurement Unit x) was originally intended to measure effeciency of Switch Mode Power Supplies hence the name REMDx, but later evolved to a datalogging device.

Concept diagram of REMDx is shown below.
![Block diagram](concept_white.png)
The REMDx PCB
![REMDx board](board_v3.jpg)

![REMDx board](remd_board_top.png)

## Capability
The REMDx is capable of measuring:
- 2 channel voltage measurement 0-35V with resolution of approximately 1mV
- 2 channel current measurement 0-8A  with resolution of approximately 250uA
- 2 channel power measurent
- 2 channel burden voltage measurent

J9 connect PINOUT
- 4 Digital input pins max. 3.3V (INTx - interrupt pins)
- 2 Analog input pins max 3.3V (MCU ADC with 12bit)
- 2 Digital output pins with selectable output voltage level (3.3V, 5V, and 12V)
- 2 PWM pins with selectable output voltage level (3.3V, 5V, and 12V)

### SD-card
Micro-SDcard slot
### Real-time plotting with python
Data stream capability using USB for datalogging and visualization with python (see scripting folder)
### Register Map
Can be configured using register map, the configuration can be saved in EEPROM (default configuration can be restored), see register map section


