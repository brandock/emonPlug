# emonPlug Overview
OpenEnergyMonitor-compatible energy monitoring plug. 

emonPlug is an energy monitoring transmitter unit that can monitor AC electrical consumption of one appliance connected to its NEMA 5-15R receptacle. 

emonPlug is designed to be used together with an emonBase web-connected base station from OpenEnergyMonitor. It can be thought of as an emonTx+emonVs for monitoring one appliance.

# emonPlug Architecture
emonPlug uses a CT to perform non-invasive current monitoring, similar to emonTx from OpenEnergyMonitor. https://docs.openenergymonitor.org/emontx3/index.html

It uses a voltage transformer for voltage monitoring and includes a built-in power supply, similar to emonVs from OpenEnergyMonitor. https://docs.openenergymonitor.org/emontx4/voltage_sensors.html

It is firmware compatible with emonTx3 (with appropriate calibration settings). emonPlug-specific firmware, based on the emonTx3 firmware, is provided in this repository.

The CT is an AC1015 from Talema Group. It is rated to 15A with 1000:1 turn ratio.

The voltage transformer is the ZMPT101B. This is the same transformer used in the emonVs. The current emonPlug is US-specific, so the 3x10K, 1/4W current-limiting resistors are designed to maximize the waveform of 120V AC. More resistance, more power dissipation, and a different burden resistor would be necessary for anyone modifying emonPlug for use with 240V AC outside the US.

emonPlug uses the Polycase PM2425 AC wall plug enclosure with US 15 AMP 120 VAC plug configuration. https://www.polycase.com/pm2425.

The emonPlug MCU is the Microchip ATMega328 and the radio is the HopeRF RFM69CW, similar to emonTx3.

![emonCMS_output](https://github.com/brandock/emonPlug/assets/17953028/205c7103-57a2-43d5-aad7-303de3be4c93)


![pm2425](https://github.com/brandock/emonPlug/assets/17953028/2f35ab60-ba14-44eb-bf13-9e893ec02936)

![IMG_3885](https://github.com/brandock/emonPlug/assets/17953028/8c611282-87d1-4add-9055-7dc4890a0534)
![IMG_3900](https://github.com/brandock/emonPlug/assets/17953028/7241f270-b041-430c-9e75-b5aa29b76e3c)
