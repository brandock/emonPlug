# emonPlug Overview
OpenEnergyMonitor-compatible energy monitoring plug. 

emonPlug is an energy monitoring transmitter unit that can monitor AC electrical consumption of one appliance connected to its NEMA 5-15R receptacle. 

emonPlug is designed to be used together with an emonBase web-connected base station from OpenEnergyMonitor. It can be thought of as an emonTx+emonVs for monitoring one appliance.

Make sure to read the warning and disclaimer below before starting any work on emonPlug assembly or use. 

# emonPlug Architecture
emonPlug uses a CT to perform non-invasive current monitoring, similar to emonTx from OpenEnergyMonitor. https://docs.openenergymonitor.org/emontx3/index.html

It uses a voltage transformer for voltage monitoring and includes a built-in power supply, similar to emonVs from OpenEnergyMonitor. https://docs.openenergymonitor.org/emontx4/voltage_sensors.html

It is firmware compatible with emonTx3 (with appropriate calibration settings). emonPlug-specific firmware, based on the emonTx3 firmware, is provided in this repository.

# emonPlug Parts
The CT is an AC1015 from Talema Group. It is rated to 15A with 1000:1 turn ratio.

The voltage transformer is the ZMPT101B. This is the same transformer used in the emonVs. The current emonPlug is US-specific, so the 3x10K, 1/4W current-limiting resistors are designed to maximize the waveform of 120V AC. More resistance, more power dissipation, and a different burden resistor would be necessary for anyone modifying emonPlug for use with 240V AC outside the US.

emonPlug uses the Polycase PM2425 AC wall plug enclosure with US 15 AMP 120 VAC plug configuration. https://www.polycase.com/pm2425.

The emonPlug MCU is the Microchip ATMega328 and the radio is the HopeRF RFM69CW, similar to emonTx3.

# emonPlug Photos

![emonCMS_output](https://github.com/brandock/emonPlug/assets/17953028/205c7103-57a2-43d5-aad7-303de3be4c93)


![pm2425](https://github.com/brandock/emonPlug/assets/17953028/2f35ab60-ba14-44eb-bf13-9e893ec02936)

![IMG_3885](https://github.com/brandock/emonPlug/assets/17953028/8c611282-87d1-4add-9055-7dc4890a0534)
![IMG_3900](https://github.com/brandock/emonPlug/assets/17953028/7241f270-b041-430c-9e75-b5aa29b76e3c)

# Warning and Disclaimer
Make sure to read this disclaimer before starting any work on emonPlug assembly or use. Credit to Felix Rusu of LowPowerLab for the wise wording of this advice and disclaimer.

emonPlug is isolated from mains electricity via the RECOM PSU (3kV isolation). Additional safety devices in the emonPlug include a current limiting fuse and a MOV (metal oxide varistor) device to help protect against transients.

Electricity WARNING: whenever you touch anything that is or was connected to mains voltages you have to really know what you are doing, otherwise you may shock yourself to shivers. If you are not 100% sure, do yourself a favor and don’t touch anything. ATX power supplies, TVs, CRT/LCD monitors, among most other high voltage devices contain very large capacitors that can hold lethal charges that may or may not discharge after the device is unplugged. You should discharge these caps before touching the circuit.

You should not attempt to assemble or use an emonPlug if you are not familiar with mains wiring! Mains voltage can cause a fatal shock or start a fire when used improperly or negligently! Please consult this electrical safety handbook to understand how you can protect yourself and your equipment from electrical shock.

The emonPlug is not an end consumer product but an open-source design that requires assembly/soldering and is intended for users comfortable/qualified to assemble it. Certain parts are polarized and require proper orientation before soldering otherwise emonPlug may malfunction or not work.
emonPlug also requires programming prior to it being functional. The designer of emonPlug is not liable for how the end user decides to assemble or use and customize the sample firmware code. The information and sample code is meant as an educational resource only, to help guide the end user with assembly and better understand how emonPlug can operate. The end user is fully responsible to ensure the final product will be electrically safe to operate.
Although as a whole, emonPlug is not UL/CSA/CE certified. It uses a certified power supply. However emonPlug comes with NO GUARANTEES expressed or implied and the designer is not liable for any injuries or damage caused by improper assembly or use of emonPlug. It may be against code to install non UL certified equipment in your area, so you are responsible to make sure you are compliant or take any risks on your own. If you do not agree or understand the terms of this disclaimer you should not use emonPlug. Use it at your own risk.
