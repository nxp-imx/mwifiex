[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 17.92.1.p136.13 to 17.92.1.p136.24

None.

## Firmware version 17.92.1.p136.24 to 17.92.1.p136.131

None.

## Firmware version 17.92.1.p136.131 to 17.92.1.p136.132

None.

## Firmware version 17.92.1.p136.132 to 17.92.1.p149.131

None.

## Firmware version 17.92.1.p149.131 to 17.92.1.p149.43

Bluetooth

- After Bluetooth firmware is downloaded on in-band reset, sometimes the firmware fails to respond to HCI reset and is not able to bring up Bluetooth interface.

## Firmware version 17.92.1.p149.43 to 17.92.1.p149.156

Bluetooth

- After Bluetooth firmware is downloaded on in-band reset, sometimes the firmware fails to respond to HCI reset and is not able to bring up Bluetooth interface.

## Firmware version 17.92.1.p149.156 to 17.92.1.p149.53

None.

## Firmware version 17.92.1.p149.53 to 17.92.1.p149.60

None.

## Firmware version 17.92.1.p149.53 to 17.92.1.p149.60

Wi-Fi

- Firmware command timeout error \[0xb\] observed during long-term stress testing of legacy roaming.
- Firmware command timeout error \[0x107\] observed during long-term stress testing of legacy roaming.
- TX timeout error observed during long-term stress testing of legacy roaming.
- Firmware crash observed in the long run test when AP is enabled and STA is connected to external AP.

## Firmware version 17.92.1.p149.60 to 17.92.1.p149.76

**Wi-Fi**
- Wakeup card timeout and background scan timeout reported following association failure.
- Kernel panic observed upon removal of the SD interface after netmon configuration.

# Firmware version 17.92.1.p149.76 to 17.92.1.p149.77

**Wi-Fi**
- Firmware dump triggered automatically during profile switch due to WL_AHB_TIMEOUT on DUTSTA.
- Unable to set the txratecfg settings when antcfg is configured on rtap interface.
- Firmware dump triggered during suspend-resume cycle when gtk_rekey_offload=2 is set.
- Incorrect noise floor values observed in open environment conditions.

# Firmware version 17.92.1.p149.77 to 17.92.1.p149.81

**Wi-Fi**
- Firmware command timeout (0x25f) observed when connecting the 48th client to DUT AP on channel 36 in HE80 and WPA3-SAE configurations.
- Firmware command timeout (0x28c) occurred after issuing the foundry_type command while STAUT was in a connected state.
- In DUT STA mode, STA was sending probe request frames on disabled channels.
- Integrated tx_retries and fcs_err_count into kernel station_info for enhancement and diagnostics.
- Introduced driver load parameter for plinkstats to support Wi-Fi performance metrics.

**Parent page:** [SDIO-UART 88W9098](../topics/sd-uart_88w9098.md)

