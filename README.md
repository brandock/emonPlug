# emonPlug
OpenEnergyMonitor-compatible energy monitoring plug. 

emonPlug is an energy monitoring transmitter unit that can monitor AC electrical consumption of one appliance connected to its NEMA 5-15R receptacle. 

emonPlug is designed to be used together with an emonBase web-connected base station from OpenEnergyMonitor. It can be thought of as an emonTx+emonVs for monitoring one appliance.

# Architecture
emonPlug uses a CT to perform non-invasive current monitoring, similar to emonTx from OpenEnergyMonitor. It uses a voltage transformer for voltage monitoring and includes a built-in power supply, similar to emonVs from OpenEnergyMonitor. It is firmware compatible with emonTx3 (with appropriate calibration settings). emonPlug-specific firmware, based on the emonTx3 firmware, is provided in this repository.


![IMG_3885](https://github.com/brandock/emonPlug/assets/17953028/8c611282-87d1-4add-9055-7dc4890a0534)
![IMG_3900](https://github.com/brandock/emonPlug/assets/17953028/7241f270-b041-430c-9e75-b5aa29b76e3c)
