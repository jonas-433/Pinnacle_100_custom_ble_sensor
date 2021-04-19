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

> **Note:** See [Firmware Update](#firmware-updates) for what interfaces (UART, BLE, LTE, HTTPS) are available for each version. 

## Development

### Clone Firmware Source

This is a Zephyr `west` manifest repository. To learn more about `west` see [here](https://docs.zephyrproject.org/latest/guides/west/index.html).

To clone this repository properly use the `west` tool. To install `west` you will first need Python3.

Install `west` using `pip3`:

```
# Linux
pip3 install --user -U west

# macOS (Terminal) and Windows (cmd.exe)
pip3 install -U west
```

Once `west` is installed, clone this repository using `west init` and `west update`:

```
# Checkout the latest manifest on main
west init -m https://github.com/LairdCP/Pinnacle_100_custom_ble_sensor.git --manifest-rev custom_template

# Now, pull all the source described in the manifest
west update
```

### Prepare to Build

If this is your first time working with a Zephyr project on your computer you should follow the [Zephyr getting started guide](https://docs.zephyrproject.org/latest/getting_started/index.html#) to install all the tools.

The firmware uses zephyr 2.4.x, so GCC 9 is recommended.
[GNU Arm Embedded Toolchain: 9-2020-q2-update](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) is recommended.

See here to [setup the GNU ARM Embedded tools](https://docs.zephyrproject.org/2.4.0/getting_started/toolchain_3rd_party_x_compilers.html#gnu-arm-embedded)

If using Linux, v0.11.4 of the Zephyr SDK is recommended.

### Build Firmware (Zephyr App)

You can build firmware using either [commands](docs/firmware_update.md#building-the-firmware) or [Visual Studio Code](docs/development.md#building-the-firmware).

### Update Firmware

If the Pinnacle 100 device is running v2.0.0 firmware or earlier, firmware updates must be programmed via SWD(Serial Wire Debug). There is no FOTA capability in the 2.x releases. To perform the update this way, please consult:

- MG100: the MG100 Hardware Guide section 5.4.4 to learn how to connect a J-Link debugger to the board.
- Pinnacle 100 DVK: The DVK has a built in debugger to easily program firmware.

Pinnacle 100 devices with firmware version 3.x or greater support firmware updates via UART, BLE or LTE. Updates via LTE on 3.x firmware must be initiated from the Laird Connectivity Bluegrass cloud portal. Only images hosted by Laird Connectivity are supported on the 3.x release. On 4.x releases, updates over HTTPS can be initiated for images hosted on any AWS server.

To update firmware with the Pinnacle Connect mobile app [see here.](docs/readme_ltem_aws.md#firmware-updates)

To update firwmare via SWD, [see here](docs/firmware_update.md#firmware-updates-via-swd)

To update firmware over UART using the mcumgr CLI, [see here.](docs/firmware_update.md#update-zephyr-app-via-uart)

To update firmware over HTTPS. Updates over HTTPS have been decoupled from Laird Connectivity's Bluegrass cloud portal. Images hosted on any AWS server can be downloaded. The details on how to trigger the update via the device shadow are available [here.](docs/cloud_fota.md)

## Connect to AWS account 

Read [here](docs/aws_iot.md) where you will set up AWS account and set its endpoint address in MG100 so that you can see data from your BLE sensor in your AWS IoT. 
