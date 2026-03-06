# GPSDO Rev. 4.0 Documentation

## Overview

This document describes the 4th revision of the AERPAW GPSDO as well as the associated firmware and software. First, the hardware is described and all its function blocks are explained in detail. Then, the process of assembling and testing the GPSDO is explained. The firmware and host software, as well as the way to compile them is then finally explained.

TODO: Picture

## Hardware

This section describes the GPSDO hardware, a picture of which is shown below. A full schematic of the board can be found in the KiCad files.

TODO: Picture

- **10MHz Connector:** The 10MHz connector provides a 10MHz refernce signal for use by a USRP or other SDR.
- **1PPS Connector:** The 1-Pulse-Per-Second (1PPS) connector provides a pulse every second. This signal is 3.3V CMOS.
- **OCXO Data LED:** The OCXO Data LED blinks when data is exchanged between the OCXO module and host PC.
- **GNSS Data LED:** The GNSS Data LED blinks when data is exchanged between the GNSS module and host PC.
- **1PPS LED:** The 1PPS LED blinks once per second in phase with the 1PPS output.
- **OCXO Lock LED:** The OCXO Lock LED lights up when the OCXO has been able to lock onto the 1PPS signal from the GNSS module.
- **Power LED:** The power LED lights up any time the board receives power from either USB or the power connector.
- **USB Connector:** The USB connector allows a host computer to communicate with the on-board GNSS and OCXO modules.
- **Power Connector:** The power connector is used to provide 5 to 15V to the GPSDO.

### Theory of Operation

This section describes the inner workings of the GPSDO hardware, a block diagram of which is shown below. First, the power section and various safety features are described. Then, the GNSS and OCXO modules are described and the choice to use an external GNSS module is explained. Finally, the microcontroller section and its components is described.

TODO: Block diagram

#### Power Section

The power section takes 5 to 15V, either from the dedicated power connector or USB, filters it to attenuated noise from the incoming supply and regulates it down to 3.3V. The filter is implemented as simple common-mode choke. The power section also includes a series of protection features to ensure reliability and resiliance to mishandling. The incoming power is directed through a Schottky diode, a polyfuse and then is clamped by a 15V TVS diode. In the event that the board is powered in the wrong polarity, the board will simply not power on and no damage will be made. In the event that the 15V voltage limit is exceeded, the the TVS diode clamps it and the high resulting high current trips the polyfuse. Since polyfuses self reset when power is removed for a certain amount of time, no lasting damage is done. The schematic of the power section is as shown below.

TODO: Schematic.

#### GNSS Module

The GNSS module is an off-the-shelf SparkFun ZED-F9T breakout board. It's job is to receive GPS signals from the antenna connected to its input and generate a 1-Pulse-Per-Second (1PPS) signal that the OCXO module discussed further down can lock onto to generate a stable 10MHz. The module communicates with the microcontroller over a UART connection operating at 38400baud 8n1 (8bits, no parity, 1 stop bit).

#### OCXO Module

The OCXO module is actually originally an entire GPSDO in itself. Unfortunately, the GNSS portion of it was not satisfactory, so the external GNSS module is used to provide it with a 1PPS signal that it can lock onto. The module then generates a 10MHz signal from which it generates its own 1PPS signal. By comparing the phase of its 1PPS signal to the one from the GNSS module, it is able to steer the frequency of its oscillator slowly until the 1PPS signals are phase locked together. When this occurs, the Lock LED on the board lights up and the GPSDO is ready for operation. The OCXO module communicates with the microcontroller over a UART connections operating at 115200baud 8n1.

#### Microcontroller

The microcontroller is a Raspberry Pi RP2040. Its job is to relay the communication from the GNSS and OCXO modules to the host computer through a USB interface. It loads its firmware from the on-board flash memory and gets its clock from a separate 12MHz oscillator.

### Assembly

This section describes the process of assembling the GPSDO hardware from blank PCBs and components. This process can be summarized as follows: First, solder paste is applied to the PCB with the help of a stencil. Next, the intergrated circuits are placed, followed by small passives, then larger passives and finally the SMA connectors. The board can then be reflowed, after which the through-hole components can be soldered in manually. At this stage, the 3.3V voltage rail is checked and, if within the allowable range, the GNSS and OCXO modules are installed.

In general, it is recommended to have a computer on hand with the KiCad files open to be able to quickly find the location and orientation of all components. It is also helpful to figure out which traces carry which signals if debugging is necessary.

#### Solder Paste Application

The solder paste is applied using a solder paste stencil. This is simply a thin piece of stainless steel with precisely cut holes which are designed to let a specific quantity of solder paste in specific areas. To use the stencil, it is recommended to build a holding jig with other PCBs to hold the blank GPSDO PCB in place while the stencil is lowered onto it and solder paste is applied as shown in the picture below.

TODO: Picture

The paste can then be applied by first laying down a beed of solder paste onto one side of the stencil and dragging it across using a Squeegee as sown in the picture below.

TODO: Picture

The stencil can then be lifted revealing the paste applied to the pads on the PCB.

#### Placing the Integrated Circuits

Once the solder paste has been applied, the first components to be placed are the integrated circuits. This is done because they have a very small pin pitch and thus require a decent amount of precision to place. Having other components already on the board could make this more difficult. The board was designed such that all ICs are in the exact same orientation, with pin 1 oriented towards the top-left corner of the board, aka. the corner with the large power connector. The integrated circuits to place are as shown in the picture below.

TODO: Picture

#### Placing the Small Passives

After the integrated circuits are placed, all the 0805-sized passives can be placed. This includes all decoupling capacitors around the MCU, all resistors, a few small diodes and the polyfuse. The small passives are shown in the picture below.

TODO: Picture

#### Placing the Large Passives

The next components to be placed are the larger passives. This includes the inductors, the large TVS diode and the large decoupling capacitors for the switching voltage regulator. These large passives are as shown in the picture below.

TODO: Picture

#### Placing the SMA Connectors

Once all other SMD components have been places, the SMA connectors can be placed. To do this, simply slide the connectors over the edge of the PCB and make sure they are aligned properly with the pads as shown in the picture below.

TODO: Picture

#### Reflowing

With all SMD components now placed, the board can be reflowed. This involves heating up the board to melt the solder paste. To do this, places the PCBs in the reflow oven with dummy PCBs serving as spacers to make sure the PCBs do not touch the botton of the reflow oven as shown in the picture below. Then, close the oven tray and start the reflow process.

TODO: Picture

Once the oven indicates that the reflow process is over, open the tray and wait a few minutes for the board to cool down further.

#### Checking for Solder Bridges

Manual solder paste application and component placement is not perfect. After reflow soldering, it is likely that solder bridges will be left, most often on the pins of the integrated circuits. If any bridges are found, DO NOT proceed further until they are remedied as described below.

#### Soldering the Through-Hole Components

Now that all SMD components have been soldered using reflow soldering, the through-hole components can be soldered manually. The throughole components are as shown in the picture below.

TODO: Picture

> [!CAUTION]
> DO NOT install the GNSS and OCXO modules at this stage. The voltage regulator could be defective and cause a high voltage to appear on the 3.3V rail. This could easily destroy the expensive GNSS and OCXO modules.

#### Verifying the 3.3V Rail

With all components solder, the board can now be powered from the power connector. A multimeter is then used to measure the 3.3V rail at the point shown in the picture below. The measured voltage must be in the range of 3.2 to 3.4V for the board to be considered working at this stage.

TODO: Picture

#### Ultrasonic Cleaning

Once the power section has been confirmed to be working, the board can be cleaned using the ultrasonic cleaner to remove all flux residue.

#### Installing the GNSS and OCXO modules

The last step in the assembly process is simply installing the GNSS and OCXO modules. While the GNSS module is straight foward to install, caution must be used when installing the OCXO module due to its thin pins that easily bend and break.

### Bring-Up

With the GPSDO fully assembled, power is applied through the power connector and the USB port is connected to a computer. The GPSDO should appear as a USB flash drive.

#### Flashing the firmware

To flash the firmware, simply drag and drop the .UF2 firmware file to the virtual USB flash drive. The board should then reboot, after which you should see two virtual serial ports.

#### Configuring the OCXO Module

Out of the box, the OCXO modules are not configured to lock from an external 1PPS. To configure this, open the serial port corresponding to the OCXO module in a virtual terminal and run the following commands:

```
SYNC?
SYNC:SOUR:MODE EXT
SYNC?
```

This configuration is stored in non-volatile memory on the module and thus only has to be done once. After this, the OCXO module should now attempt to lock onto the 1PPS signal from the GNSS module.

#### Troubleshooting

TODO

### Testing

TODO

## Firmware

This section describes the firmware of the GPSDO, that is the code that runs on the embedded microcontroller.

### Theory of Operation

### Compilation

## Software

### Theory of Operation

### Compilation
