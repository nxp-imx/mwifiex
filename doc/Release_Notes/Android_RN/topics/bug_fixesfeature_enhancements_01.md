[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 17.92.1.p136.13 to 17.92.1.p136.24

Coexistence

-   OPP file transfer gets failed while OPP file transfer is ongoing and Wi-Fi traffic initiated with 2.4GHz external AP.

## Firmware version 17.92.1.p136.24 to 17.92.1.p136.131

Wi-Fi

-   Wake-up card timeout is seen when performing suspend &amp; resume stress test with iMX8 host.

## Firmware version 17.92.1.p136.131 to 17.92.1.p136.132

None.

## Firmware version 17.92.1.p136.132 to 17.92.1.p149.131

Bluetooth

-   A2DP Audio glitches heard while audio streaming and OPP file transfer to another reference device at the same time.

## Firmware version 17.92.1.p149.131 to 17.92.1.149.43

Bluetooth

-   After Bluetooth firmware is downloaded on in-band reset, sometimes the firmware fails to respond to HCI reset and is not able to bring up Bluetooth interface.

## Firmware version 17.92.1.p149.43 to 17.92.1.149.156

None.

## Firmware version 17.92.1.p149.156 to 17.92.1.149.53

None.

## Firmware version 17.92.1.p149.53 to 17.92.1.149.60

None.

## Firmware version 17.92.1.p149.60 to 17.92.1.p149.70

Bluetooth

-   During firmware initialization, any pulse on UART TX line leads to firmware initialization failure
-   After braktooth attack of duplicated encapsulated payload, DUT not able to start the Page and Inquiry scan.

# Firmware version 17.92.1.p149.70 to 17.92.1.p149.76

**Wi-Fi**
- DUT sends a second Reassociation Request even after receiving an acknowledgment for the initial Reassociation Request.
- Observed 30–40% throughput degradation when MTU size is configured to 1532 bytes.

# Firmware version 17.92.1.p149.76 to 17.92.1.p149.77

**Wi-Fi**
- Incorrect noise floor values observed in open environment conditions.

# Firmware version 17.92.1.p149.77 to 17.92.1.p149.81

**Wi-Fi**
- Firmware command timeout (0x25f) observed when connecting the 48th client to DUT AP on channel 36 in HE80 and WPA3-SAE configurations.
- Firmware command timeout (0x28c) occurred after issuing the foundry_type command while STAUT was in a connected state.
- In DUT STA mode, STA was sending probe request frames on disabled channels.
- Integrated tx_retries and fcs_err_count into kernel station_info for enhancement and diagnostics.
- Introduced driver load parameter for plinkstats to support Wi-Fi performance metrics.

**Bletooth**
- Invalid HCI Command Parameters received for LE Set Extended Advertising Data.

**Parent file:**[PCIe-UART 88W9098](../topics/pcie-uart_88w9098.md)

