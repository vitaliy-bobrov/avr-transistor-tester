# AVR Transistor Tester

Component/Transistor tester based on famous [AVR Transistor Tester](https://www.mikrocontroller.net/articles/AVR_Transistortester) project. The reason I decided to create a custom PCB is that currently there is not good kit or complete device available that would fit my wishes:

- ATMega644 MCU
- 2.4" 65K colors LCD
- Rotary encoder
- USB-C charger with Li-Po battery
- Higher grade/precision components

Most of Chinese versions are starting to use fake or non-original Microchip MCUs that prevents their software upgrades and economy on components makes them less accurate. I didn't include a lot of hardware options, because I primarily using the device for measuring passive components and transistors used in guitar pedals building.

## Complete unit

![](https://github.com/vitaliy-bobrov/avr-transistor-tester/blob/main/images/ttester-complete.jpg)

## PCB

I used SMD assembly service at JLCPCB and ended up with a set 5 PCBs cost around 25$ each. But since SMD components used there is not the smallest size ones, it is 100% possible to solder everything by hand.

![](https://github.com/vitaliy-bobrov/avr-transistor-tester/blob/main/images/ttester-pcb-components.jpg)
![](https://github.com/vitaliy-bobrov/avr-transistor-tester/blob/main/images/ttester-pcb-controls.jpg)

## Schematic

[PDF](https://github.com/vitaliy-bobrov/avr-transistor-tester/blob/main/schematic.pdf)

![](https://github.com/vitaliy-bobrov/avr-transistor-tester/blob/main/images/schematic.png)

## Hardware

PCB has almost everything as SMD assembly and requires a few components to be soldered by hand:

- Rotary Encoder - basically any common one would work here, but I've used BOURNS PEC11R-4220K-S0024. It is available at most big components suppliers. My personal choice was [TME](https://www.tme.eu/pl/details/pec11r-4220k-s0024/enkodery-inkrementalne/bourns/).
- ZIF 14 pin socket - again you could use any common one. I used CONNFLY DS1044-140G from [TME](https://www.tme.eu/pl/details/ds1044-140g/podstawki-testowe/connfly/)
- 2.4" 65K colors LCB. Not sure if other ILI9341 controller based would have the same pins/size. I used [Waveshare 18366](https://www.waveshare.com/2.4inch-lcd-module.htm) which is not expensive and commonly available. At [TME](https://www.tme.eu/pl/details/wsh-18366/wyswietlacze-lcd-graficzne/waveshare/18366/) too ;)
- 2.54mm spaced 2x3 pins for ISP - this could be left without pins soldered, then just ensure you have good contact between the PCB and programming pins.
- 2.54mm 3 pin socket for components/transistors - just alternative for ZIF socket
- Any Li-Po or Li-Ion battery with work with the charger IC, but the PCB was designed with Li-Po in mind due to size. I used [550mA battery](https://www.tme.eu/pl/details/aky-lp503040/akumulatory/akyga-battery/aky0185/), but you could use any other that would fit about 55x50mm size.

## Software

The software used in modified m-software version 1.56m (see [docs](https://github.com/madires/Transistortester-Warehouse/tree/master/Documentation)), but k-software could be used as well. The pins assignment is based on project docs suggested one. Precompiled version is in the [software](https://github.com/vitaliy-bobrov/avr-transistor-tester/blob/main/software/) folder.

### Software compilation

1. Install AVR tools for your operating system and programmer you are using (ex. avrdude)
2. Navigate to software folder
3. Run `make`
4. Connect programmer to the ISP pins on the PCB
5. Run `make fuses` to set MCU fuses
6. Run `make upload` to upload firmware

## Enclosure

I designed a simple 3D printable enclosure that fits the PCB and provides easy access to ISP pins and USB-C charger. STEP files are located in the [3d-enclosure](https://github.com/vitaliy-bobrov/avr-transistor-tester/blob/main/3d-enclosure/) folder. Printing profile is available at [MakerWorld](https://makerworld.com/en/models/883799#profileId-838469) which would be handy if you have a BambuLab 3D printer.

The assembled PCB with display is mounted to enclosure using spacers that came with Waveshare display using 4 x M2x8mm and 4 x M2x16mm screws. the battery is fixed on the backplate with double sided 3M foam.
