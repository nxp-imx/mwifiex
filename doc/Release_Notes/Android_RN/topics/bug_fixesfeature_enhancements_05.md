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

# Firmware version 17.92.1.p149.81 to 17.92.1.p149.84

**Wi-Fi**
- Resolved kernel crash during Wi-Fi power on/off recovery cycle in suspend-to-RAM (STR) tests
- Fixed kernel crash during Wi-Fi reset triggered by EAPOL timeout by adding driver status checks
- Fixed cross-compilation issues on customer platforms by resolving __aeabi_uldivmod dependency
- Fixed EAPOL Key Handshake failure during roaming caused by missing M2 key transmission
- Resolved uAP/GO interface abnormal behavior after channel switch when STA connected to 40MHz AP
- Fixed missing probe request transmission from STA during roaming leading to roaming failure
- Implemented mitigation for Aruba unicast 0xf frames to prevent CarPlay disconnections
- Corrected uAP deauth reason code to comply with IEEE standards for STA age-out scenarios

**Bluetooth**
- Added support for new Bluetooth qualification test cases introduced by TCRL package 100
- After establishing a connection with the mobile device, the controller stops sending NOCP events to the host.
- BT host stuck on BT pairing process because of the missing HCI_Simple_Pairing_Complete event from the controller.

# Firmware version 17.92.1.p149.84 to 17.92.1.p149.115

**Wi-Fi**
- Fixed command timeout for RSSI_INFO (0xa4) command by ensuring it reaches firmware correctly 
during roaming operations.
- Fixed wakeup card timeout occurring after IP address acquisition when only 2.4GHz band is enabled
- Added probe request sequence number randomization in driver/firmware to enhance privacy.
- Fixed ADMA TX deadlock causing 0xa4 (RSSI_INFO) command timeout during roaming.
- Fixed STA frequently disconnecting from 5GHz and switching to 2.4GHz.
- Fixed ADMA AHB exception causing wakeup card timeout and firmware reset failure.
- Resolved link loss regression introduced by the ping latency firmware fix.
- Fixed firmware download failure during bootup stress test.
- Resolved missing probe responses in scan results causing fewer APs to be detected.
- Fixed 0xa4 command not reaching firmware, causing command timeout, by resolving ADMA race 
condition and adding timeout protection.
- Fixed command timeout in ADMA TX path.
- Resolved interleaved TSF timestamps reported when two APs are running simultaneously.
- Fixed memory use-after-free in WLAN driver causing SMMU fault.
- Fixed high ping latency observed after startup on 2.4GHz in MIMO configuration.

**Bluetooth**
- Fixed random eSCO/HFP audio glitch on PCM/TDM interface during audio playback.
- Resolved BLE disconnection at low RSSI when using LE Coded PHY S=8 (long range) data rate.
- Fixed DUT failing to reconnect due to controller being unable to resolve RPA after Bluetooth 
suspend and resume.
- Resolved HFP audio quality issue when DUT operates as slave after role switch.
- Fixed DUT becoming unable to connect after receiving a BT exploit attack with invalid timing 
accuracy.
- Fixed missing HCI_Simple_Pairing_Complete event after ACL link disconnection, which caused host 
to get stuck during BT pairing.
- Resolved BT inquiry results not appearing immediately during concurrent A2DP streaming and BLE 
scan results were delayed by ~1 minute.


**Parent page:** [SDIO-UART 88W9098](../topics/sd-uart_88w9098.md)

