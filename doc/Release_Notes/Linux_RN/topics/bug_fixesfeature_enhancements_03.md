[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 17.92.1.p98.1 to 17.92.1.p116.1

None.

## Firmware version 17.92.1.p116.1 to 17.92.1.p136.13

None.

## Firmware version 17.92.1.p136.13 to 17.92.1.p136.24

**Coexistence**
- OPP file transfer gets failed while OPP file transfer is ongoing and Wi-Fi traffic initiated with 2.4GHz external AP.

## Firmware version 17.92.1.p136.24 to 17.92.1.p136.131

**Bluetooth**
- A2DP Audio glitches heard while audio streaming and OPP file transfer to another ref device at the same time.

## Firmware version 17.92.1.p136.131 to 17.92.1.p149.131

**Wi-Fi**
- During the Roaming stress test, a command timeout causing the device Hang/Crash is observed.

## Firmware version 17.92.1.p149.131 to 17.92.1.p149.43

None.

## Firmware version 17.92.1.p149.43 to 17.92.1.p149.157

**Wi-Fi**
- In RF test mode, Inconsistent TX-power observed between configured and measured values in txcontinuous Carrier Suppression \(CS\) mode.
- In RF test mode, EVM value degradations are seen on the DFS channels with Linux BSP v6.6.23

**Bluetooth**
- In legacy remote devices, pairing with PIN code method is failed with LMP/LL timeout.

## Firmware version 17.92.1.p149.157 to 17.92.1.p149.53

None.

## Firmware version 17.92.1.p149.53 to 17.92.1.p149.60

**Wi-Fi**
- Fixed kernel panic fetal error due to connecting the DUT STA to a long SSID Ex-AP.<br/>
- In DUT STA mode, a wake-up card timeout is observed when Ex-AP is momentarily offline and when there is a connection times out.
- In DUT STA mode, the firmware command 0xd0 timeout is observed in a stress testing with good RSSI AP connection.

**Bluetooth**
- In extended Bluetooth LE scanning, an unexpected HCI\_LE\_Scan\_Timeout event is observed.

## Firmware version 17.92.1.p149.60 to 17.92.1.p149.64

**Wi-Fi**
- During firmware automatic recovery test, kernel crash causing system reboot due to Wi-Fi driver on a specific platform running in DUT STA mode.
- Readback of `txratecfg` command not showing expected output.
- In DUT STA mode, the Reassociation response from the External Access Point was dropped after the acknowledgement was sent, which resulted in another Reassociation request, ultimately leading to Deauthentication from the External Access Point.
- Firmware crash occurrence in the 2.4 GHz band during roaming stress test scenario.

## Firmware version 17.92.1.p149.64 to 17.92.1.p149.70

**Wi-Fi**
- Firmware command timeout error \[0xb\] observed during long-term stress testing of legacy roaming.
- Firmware command timeout error \[0x107\] observed during long-term stress testing of legacy roaming.
- TX timeout error observed during long-term stress testing of legacy roaming.<br/>
- Firmware crash observed in the long run test when AP is enabled and STA is connected to ext. AP.
- Linux kernel panic with Netlink hung task observed randomly while booting and shutting down host system.

**Bluetooth**
- During firmware initialization, the process fails if any pulse is observed on the UART TX line.
- After braktooth attack of duplicated encapsulated payload, DUT not able to start the Page and Inquiry scan.

## Firmware version 17.92.1.p149.70 to 17.92.1.p149.72

**Wi-Fi**
- Easymesh is enabled
- Firmware command timeout 0x249 observed sometimes during stress test of roaming on specific platform
- Fixed random kernel crash while performing stress test of automatic firmware recovery.

## Firmware version 17.92.1.p149.72 to 17.92.1.p149.74

**Wi-Fi**
- Added CSI - Ambient Motion Index (AMI) feature support.

## Firmware version 17.92.1.p149.74 to 17.92.1.p149.76

**Wi-Fi**
- Wakeup card timeout and background scan timeout reported following association failure.
- In monitor mode, RSSI readings sometimes show positive values when the signal strength is close to the ambient noise level.
- DUT sends a second Reassociation Request even after receiving an acknowledgment for the initial Reassociation Request.
- Observed 30–40% throughput degradation when MTU size is configured to 1532 bytes.
- Kernel panic observed upon removal of the SD interface after netmon configuration.

# Firmware version 17.92.1.p149.76 to 17.92.1.p149.77

**Wi-Fi**
- Firmware dump triggered automatically during profile switch due to WL_AHB_TIMEOUT on DUTSTA.
- Unable to set the txratecfg settings when antcfg is configured on rtap interface.
- Firmware dump triggered during suspend-resume cycle when gtk_rekey_offload=2 is set.
- Incorrect noise floor values observed in open environment conditions.

# Firmware version 17.92.1.p149.77 to 17.92.1.p149.81

**Wi-Fi**
- Antenna configuration (antcfg) was not taking effect in sniffer mode when no connection was present
- Firmware command timeout (0x25f) observed when connecting the 48th client to DUT AP on channel 36 in HE80 and WPA3-SAE configurations.
- Firmware command timeout (0x28c) occurred after issuing the foundry_type command while STAUT was in a connected state.
- In DUT STA mode, STA was sending probe request frames on disabled channels.
- Integrated tx_retries and fcs_err_count into kernel station_info for enhancement and diagnostics.
- Introduced driver load parameter for plinkstats to support Wi-Fi performance metrics.

# Firmware version 17.92.1.p149.81 to 17.92.1.p149.84

**Wi-Fi**
- Resolved kernel crash during Wi-Fi power on/off recovery cycle in suspend-to-RAM (STR) tests
- Fixed kernel crash during Wi-Fi reset triggered by EAPOL timeout by adding driver status checks
- Fixed kernel panic during module removal (rmmod) after interface deletion
- Fixed cross-compilation issues on customer platforms by resolving __aeabi_uldivmod dependency
- Fixed EAPOL Key Handshake failure during roaming caused by missing M2 key transmission
- Resolved uAP/GO interface abnormal behavior after channel switch when STA connected to 40MHz AP
- Fixed firmware crash triggered during RF test mode TX operations
- Added support for new Bluetooth qualification test cases introduced by TCRL package 100
- Fixed missing probe request transmission from STA during roaming leading to roaming failure
- Resolved firmware watchdog exceptions occurring in roaming scenarios
- Implemented mitigation for Aruba unicast 0xf frames to prevent CarPlay disconnections
- Corrected uAP deauth reason code to comply with IEEE standards for STA age-out scenarios

**Bletooth**
- After establishing a connection with the mobile device, the controller stops sending NOCP events to the host.
- BT host stuck on BT pairing process because of the missing HCI_Simple_Pairing_Complete event from the controller.

# Firmware version 17.92.1.p149.84 to 17.92.1.p149.88

**Wi-Fi**
- Fixed wakeup card timeout (ADMA AHB Exception) and firmware reset failure by adding power save protection around ADMA operations

# Firmware version 17.92.1.p149.88 to 17.92.1.p149.115

**Wi-Fi**
- Fixed command timeout for RSSI_INFO (0xa4) command by ensuring it reaches firmware correctly during roaming operations.
- Fixed wakeup card timeout occurring after IP address acquisition when only 2.4GHz band is enabled.
- Added probe request sequence number randomization in driver/firmware to enhance privacy.
- Fixed ADMA TX deadlock causing 0xa4 (RSSI_INFO) command timeout during roaming.
- Fixed eCSA not executing when triggered via hostapd_cli
- Fixed STA frequently disconnecting from 5GHz and switching to 2.4GHz.
- Fixed ADMA AHB exception causing wakeup card timeout and firmware reset failure.
- Fixed link loss regression introduced by the ping latency firmware fix.
- Fixed firmware download failure during bootup stress test.
- Fixed missing probe responses in scan results causing fewer APs to be detected.
- Fixed 0xa4 command not reaching firmware, causing command timeout, by resolving ADMA race condition and adding timeout protection.
- Fixed command timeout in ADMA TX path.
- Fixed interleaved TSF timestamps reported when two APs are running simultaneously.
- Fixed memory use-after-free in WLAN driver causing SMMU fault.
- Fixed high ping latency observed after startup on 2.4GHz in MIMO configuration.

**Bluetooth**
- Fixed random eSCO/HFP audio glitch on PCM/TDM interface during audio playback.
- Fixed missing Disconnect Complete event from controller causing host to get stuck after ACL link disconnection.
- Fixed random eSCO/HFP audio glitch on PCM interface during ringtone audio playback.
- Fixed BLE disconnection at low RSSI when using LE Coded PHY S=8 (long range) data rate.
- Fixed DUT failing to reconnect due to controller being unable to resolve RPA after Bluetooth suspend and resume.
- Fixed HFP audio quality issue when DUT operates as slave after role switch.
- Fixed DUT becoming unable to connect after receiving a BT exploit attack with invalid timing accuracy.
- Fixed missing HCI_Simple_Pairing_Complete event after ACL link disconnection, which caused host to get stuck during BT pairing.
- Fixed BT inquiry results not appearing immediately during concurrent A2DP streaming and BLE scan — results were delayed by ~1 minute.

# Firmware version 17.92.1.p149.115 to 17.92.1.p214.1

**Wi-Fi**
- Fixed STA transmitting unencrypted data frames during roaming.