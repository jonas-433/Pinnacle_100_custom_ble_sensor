[![Laird Connectivity](docs/images/LairdConnnectivityLogo_Horizontal_RGB.png)](https://www.lairdconnect.com/)

# Testing your own BLE sensor with MG100 on your own AWS

[![Pinnacle 100 Modem](docs/images/pinnacle_100_modem.png)](https://www.lairdconnect.com/wireless-modules/cellular-solutions/pinnacle-100-cellular-modem) [![Pinnacle 100 DVK](docs/images/450-00010-K1-Contents_0.jpg)](https://www.lairdconnect.com/wireless-modules/cellular-solutions/pinnacle-100-cellular-modem) [![MG100](docs/images/MG100-Starter-Kit.png)](https://www.lairdconnect.com/iot-devices/iot-gateways/sentrius-mg100-gateway-lte-mnb-iot-and-bluetooth-5)

## Introduction

This repository is modified from https://github.com/LairdCP/Pinnacle-100-Firmware so that MG100 can work with users' own BLE sensor on their own AWS IoT server. 

> **Note:** MG100 has two processsors - nRF52840 and HL7800 - on it and each has its own firmware. Below, 'firmware' refers to nRF52840 firmware unless it is referred specifically to HL7800 firmware. 

## Prerequisites
* [MG100](https://www.lairdconnect.com/iot-devices/iot-gateways/sentrius-mg100-gateway-lte-mnb-iot-and-bluetooth-5) with HL7800 firmware v4.3.14.0 or later
* Your own BLE sensor that broadcasts data in advert 
* Micro USB cable
* If using SWD for firmware upgrade : 
  * [Segger J-Link debugger](https://www.segger.com/products/debug-probes/j-link/models/model-overview/) (The J-Link Base or J-Link Base Compact is recommended) 
  * Programming adapter ([Tag-Connect TC2030-IDC](https://www.tag-connect.com/product/tc2030-idc-6-pin-tag-connect-plug-of-nails-spring-pin-cable-with-legs) and [ARM20-CTX Adapter](https://www.tag-connect.com/product/arm20-ctx-20-pin-to-tc2030-idc-adapter-for-cortex) are recommended) 

> **Note:** See [Firmware Update](#firmware-updates) for what interfaces (UART, BLE, LTE, HTTPS) are available per each version. 

## Firmware Updates

If the Pinnacle 100 device is running v2.0.0 firmware or earlier, firmware updates must be programmed via SWD(Serial Wire Debug). There is no FOTA capability in the 2.x releases. To perform the update this way, please consult:

- MG100: the MG100 Hardware Guide section 5.4.4 to learn how to connect a J-Link debugger to the board.
- Pinnacle 100 DVK: The DVK has a built in debugger to easily program firmware.

Pinnacle 100 devices with firmware version 3.x or greater support firmware updates via UART, BLE or LTE. Updates via LTE on 3.x firmware must be initiated from the Laird Connectivity Bluegrass cloud portal. Only images hosted by Laird Connectivity are supported on the 3.x release. On 4.x releases, updates over HTTPS can be initiated for images hosted on any AWS server.

To build firmware, [see here.](docs/firmware_update.md#building-the-firmware)

To update firmware with the Pinnacle Connect mobile app or via the Bluegrass cloud portal, [see here.](docs/readme_ltem_aws.md#firmware-updates)

To update firwmare via SWD, [see here](docs/firmware_update.md#firmware-updates-via-swd)

To update firmware over UART using the mcumgr CLI, [see here.](docs/firmware_update.md#update-zephyr-app-via-uart)

To update firmware over HTTPS. Updates over HTTPS have been decoupled from Laird Connectivity's Bluegrass cloud portal. Images hosted on any AWS server can be downloaded. The details on how to trigger the update via the device shadow are available [here.](docs/cloud_fota.md)

## Development

### Cloning and Building the Source

This is a Zephyr-based repository, **DO NOT** `git clone` this repo. To clone and build the project properly, please see the instructions in the [Pinnacle 100 Firmware Manifest](https://github.com/LairdCP/Pinnacle-100-Firmware-Manifest) repository.

### BLE Profiles

Details on the BLE profiles used to interface with the mobile app can be found [here](docs/ble.md)

### Development and Debug

See [here](docs/development.md) for details on developing and debugging this app.

## TODO 

code change + explanation 

firmware update 
- SWD
- UART
- HTTP
- BLE

Connect to AWS account
