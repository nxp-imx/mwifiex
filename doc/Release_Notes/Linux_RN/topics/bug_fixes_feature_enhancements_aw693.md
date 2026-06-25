[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 18.99.2.p145.34 to 18.99.2.p145.35

**Wi-Fi**
- Fixed random kernel crash while performing stress test of automatic firmware recovery.
- In the RFTM test, firmware crash observed on the TX tests when changing the tx power and data rate in loop.

## Firmware version 18.99.2.p145.35 to 18.99.2.p145.70

**Wi-Fi**
- Added CSI - Ambient Motion Index (AMI) feature support.

## Firmware version 18.99.2.p145.70 to 18.99.2.p145.99

**Wi-Fi**
- Command timeouts observed during roaming between two APs while running UDP Tx traffic with Video.
- Kernel crash observed on P2P-AGO when attempting to add two P2P groups operating on the same band and frequency

## Firmware version 18.99.2.p145.99 to 18.99.2.p145.147

**Wi-Fi**
- Resolved kernel panic caused by corrupted rgpower binary.
- Addressed command timeout (0x24) in STAUT during roaming stress between 2.4 GHz and 5 GHz BSS.
- Resolved Wi-Fi firmware hang during concurrent AP and STAUT operation.
- Updated VLP mode TPC for FCC to use perform back-off from customer-certified power limits instead of EIRP limits.
- Enhanced handling of SU and RU TX power table entries to support negative power levels.
- Addressed Wi-Fi throughput drop to 0 Kbps during simultaneous Bluetooth inquiry and WLAN transmission.

**Bluetooth**
- BT Crash After S2R Due to Corrupted UART Packet from Controller
- BLE Link Loss Observed During SCO Connection with HV3 Packet Type
- Role switch from Central to Peripheral fails as DUT does not transmit an ID packet in response to the FHS from Ref1, resulting in role switch fail


## Firmware version 18.99.2.p145.147 to 18.99.2.p145.172

**WI-Fi**
- Fixed kernel crash in woal_inform_bss_from_scan_result during scan result reporting and channel switching stress tests
- Fixed 6GHz PSD (Power Spectral Density) table download for Standard Power mode
- Fixed uninitialized ARP lock in woal_flush_arp_request_entry() function
- Fixed spectrum mask failure occurring after channel switch from 5GHz channel 36 to channel 100
- Fixed scan event timeout occurring during 2.4GHz HT40 operation under noisy conditions with EU adaptivity testing
- Prevented STA from switching to disabled 5GHz channels when receiving Extended Channel Switch Announcement from AP
- Fixed external coexistence WiFi channel avoidance configuration where second message in more-bit configuration failed to block expected channels

**Bluetooth**
- Resolved firmware hang when Out-of-Band deep sleep command is issued after Wake-on-BT/BLE command
- Resolved BLE connection timeout occurring after data length update procedure with specific smartphone models
- Fixed missing LL_CIS_REQ transmission during CIS connection establishment with certain Bluetooth devices
- Resolved firmware crash occurring when establishing two concurrent CIS connections in LE Audio scenarios

## Firmware version 18.99.2.p145.172 to 18.99.7.p230.2

**WI-Fi**
- DFS channels are marked as NO-IR when LTE Type MSG2 with 0xFF is received
- 0x5B command timeout during Wi-Fi STA interface initialization after a driver reload cycle
- 5GHz AP start failure when in-STA was already connected to 5GHz external AP.
- Driver compilation failures on kernel 6.1.128 due to sa_data_min reverted in Android kernel.
- Schedule scans are always using active scan even when no-IR flag is enabled in rgpower.
- Fixed: mlan driver KASAN warnings on arm64
- Fixed: 6GHz PSD table not downloaded for JP
- Disable implicit beamformer operation
- Enhancement: Add support for an additional offset Tx power table for MCS 5, 6, and 8
- Enhancement: Added support for 0.25 dBm resolution in Tx power tables loaded from Excel for the Tx Power v3 method.
- Enhancement: Enable support for antenna gain specified as decimal values in the Excel sheet with 0.25 dBm resolution.
- Enhancement: Delink Turbo mode from EDMAC With Region‑Specific Configuration
- Enhancement: Add RSSI based TPC support for ETSI and JP region for DFS channels
- Enhancement: Explicitly reject cross MAC ECSA operation (ECSA only supported within single MAC)

**Bluetooth/BLE**
- Controller responds with Command Status instead of Command Complete for Sniff Subrating command causing stack failure
- Extended Inquiry Result from remote iPhone is not send to host
- Sniff subratting event from controller is sent with all zero’s
- Delayed response from controller for HCI_Inquiry_Cancel command
- eSCO link disconnecting with reason "Connection Timeout" when packet type used is EV3 and Re-Tx as 2

**Coex**
- HFP audio becomes distorted at a Bluetooth RSSI level of -65 dBm in the presence of ongoing WLAN Tx traffic
- Sometimes Wi-fi Association failed during HFP call
- WLAN TP is reduced when HFP call ongoing and DUT is in peripheral role