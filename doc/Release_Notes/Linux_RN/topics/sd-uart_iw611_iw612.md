[Link to index page](../index.md)

# SDIO-UART IW611/IW612

## Package information

- BSP version: Linux 6.18.2\_1.0.0
- Wi-Fi and Bluetooth/Bluetooth LE Firmware version 18.99.8.p16
- Driver version: MM6X18542.p22-GPL

## Version information

- Wireless SoC: IW611/IW612
- Wi-Fi and Bluetooth/Bluetooth LE Firmware version 18.99.8.p16
    - 18 - Major revision
    - 99 - Feature pack
    - 8 - Release version
    - p16 - Patch number
- Driver Version: MM6X18542.p22-GPL
    - 6X - Linux 6.x Kernel
    - 18542 - Release version
    - p22 - Patch number
    - GPL - General Public License v2

## software release content

- Firmware binaries
- RF test mode is enabled in the production Firmware. A separate firmware binary is not required to execute RF test mode commands.

|Firmware|IW611/IW612 A1 with secure boot enabled|
|--------|---------------------------------------|
|Combo firmware without Zigbee|sduart\_nw61x\_v1.bin.se|
|Combo firmware with Zigbee|sduart\_nw61x\_v1\_zb\_dual\_pan.bin.se|
|Wi-Fi only|sd\_w61x\_v1.bin.se|
|Bluetooth & 802.15.4 only without Zigbee|uartspi\_n61x\_v1.bin.se|
|Bluetooth, Zigbee & dual PAN without LE audio|uartspi\_n61x\_v1\_zb\_dual\_pan.bin.se

## Host platform

- MCIMX8M-EVK platform running Linux
- Supported Linux kernel versions: From 2.6.32 to 6.18.2
- Interface used
    - Wi-Fi over SDIO \(SDIO 3.0 support, Clock speed: 200 MHz\)
    - Bluetooth/Bluetooth LE over UART
    - 802.15.4 over SPI interface
- Test tools
    - iPerf \(version 2.0.13\)
    - wpa\_supplicant \(version 2.11\)
    - hostapd \(version 2.11\)
