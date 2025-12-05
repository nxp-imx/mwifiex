[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 18.99.2.p19.15 to 18.99.2.p66.10

Wi-Fi

- Wake-up card timeout is observed when the DUT AP changes the channels during TWT execution.
- The DUT-STA does not stop sending the periodic null frames after executing TWT Teardown.

## Firmware version 18.99.2.p66.10 to 18.99.2.p66.18

Wi-Fi

- The DUT-AP keeps sending RTS to the external device \(in switched off state\) until the age-out timer expires.
- Firmware fatal automatic recovery failed in long run stress testing.
- DUT wakeup interval found unexpected for successive wakeups in TWT session of specific Service period which can be more than 10 minutes.
- In DUTSTA Command 802\_11\_SCAN\_EXT \[0x107\], timeout is seen when performing scan while doing Auto-TX.

Bluetooth

- When A2DP steaming is initiated during an ongoing HFP call, A2DP link loss is observed due to LMP response timeout \(frequency of occurrence 4/5 times\).
- Link instability in presence of multiple Bluetooth links under optimization.
- When Bluetooth Scatternet and eSCO link run for a long time, the DUT may hang.

## Firmware version 18.99.2.p66.18 to 18.99.3.p10.1

Wi-Fi

- STAUT does not follow the configured wake-up duration.
- DUTAP hang observed during youtube stress test within ~20 mins with ch 36 Open Noisy Environment.
- DUTSTA sends deauth due to unspecified reason and observed hang in disconnected state.
- In DUT STA mode, scan command timeout was observed during WPA3-FT PSK rssi-based roaming test on long run.

## Firmware version 18.99.3.p10.1 to 18.99.3.p15.10

Bluetooth

- When two Bluetooth ACL links are connected and HFP call is ongoing in one of these connections, and the DUT starts the scanning process, a disconnection with the second ACL link occurs.

Coexistence

- Audio cuts observed while running the DUT STA coex RVR test.
- Wi-Fi throughput goes 60% down when DUT working as slave role is connected to mobile phone.

LE audio

- In stress testing of 2-CIS, collision of ISO packet and ATT data affects ISO anchor point scheduling.
- DUT is generating BIG sync lost event randomly after some inactivity.
- Second CIS establishment always fails when creating 2 CIS over one ACL with interleaved packing.
- Sometimes the connection timeout for CIS establishment event is observed on the second CIS link while creating all 2 CIS together.

## Firmware version 18.99.3.p15.10 to 18.99.3.p21.10

Wi-Fi

- In the DRCS test, DUT AP sent fewer beacons resulting ext. STA disconnections when the DUT-STA is already performing a scan operation.

- In the DRCS test, the DUT AP sent fewer beacons resulting ext. STA disconnections when the DUT-STA is already performing a scan operation.

- In the DRCS test, connection failures are seen when a mobile tries to associate with DUT AP and DUT-STA tries to connect with an ext AP using the wrong password.

- During TX power and regulatory test, kernel warning observed when `tx-power` values are not the same for 20 MHz, 40 MHz, and 80 MHz bonded channels.


Coexistence

- Audio cuts observed while running the DUT STA coex. RVR test

## Firmware version 18.99.3.p21.10 to 18.99.3.p23.6

Wi-Fi

- Observed the DUT platform reboot issue when the external client connected to the DUT-AP is streaming, and in parallel the DUT-STA is scanning.

Coexistence

- Wi-Fi throughput numbers degraded by 35% when LE audio stream 1-BIS is running in the background.

## Firmware version 18.99.3.p23.6 to 18.99.3.p25.7

Wi-Fi

- Firmware auto recovery failures occur during long run stress test in DUT-AP mode.
- firmware command timeout error \[0x107\] observed during long-term stress testing of legacy roaming.
- TX timeout error was observed during long-term stress testing of legacy roaming.
- Firmware crash observed in the long run test when AP is enabled and STA is connected to an external AP.

Bluetooth

- DUT hang observed randomly while connected with the peer device on Bluetooth/Bluetooth LE link for a long time duration.
- When 251 bytes of data set as extended advertisement data, over the air data length is set as 242 bytes.

Coexistence

- DUT fails to start the mobile hotspot when Bluetooth and OpenThread traffic is ongoing.

## Firmware version 18.99.3.p25.7 to 18.99.3.p25.10

**Wi-Fi**
- In the DUT AP mode, DHCP timeout observed on the external STA when switching security from one enterprise to another enterprise.
- In automatic firmware recovery, random kernal crash observed during stress test.
- One of the pattern for ETSI Type 3 ETSI pulses has detection rate of 0% during the CAC period on specific setup

**Bluetooth**
- The firmware fails to assign connection handles during BIG (Broadcast Isochronous Group) creation.

## Firmware version 18.99.3.p25.10 to 18.99.3.p26.7

**Wi-Fi**
- Addressed an inconsistency in background scan reporting where RSSI values were occasionally misreported as above threshold despite actual signal strength being lower
- Addressed malfunction in offloading support for ARP, ICMP ping, and IPv6 Neighbor Solicitation.

**Bluetooth**
- Fixed the issue where the DUT did not respond to the LL_FEATURE_REQ command.
- Fixed a delay in the first eSCO connection caused by the controller not transmitting max_slot and max_slot_req over the air.
- Fixed an issue where no Disconnection Complete event was received for eSCO from the controller, causing the controller to enter a hang state.

**Wi-Fi and Bluetooth coexistence**
- Wi-Fi command timeout is seen when there is A2DP streaming from phone.

## Firmware version 18.99.3.p26.7 to 18.99.3.p26.10

**Wi-Fi**
- FW crash is observed during RFTM TX.
- False radar detections (2/10) observed during full-buffer UDP Tx traffic in JP W53 region under HE80 mode.
- Firmware dump triggered when RF test mode configurations are applied

## Firmware version 18.99.3.p26.10 to 18.99.3.p26.11

**Wi-Fi**
- False secondary GI detection and throughput drop observed when duplicate RTS feature is enabled.
- uAP and STA fail to perform ranging in associated mode under FTM.
- Added validations for bandwidth configurations to avoid user errors in FTM Request.

**Bluetooth**
- Fixed issue where ISCAN/PSCAN re-enable was ineffective due to immediate disable after connection.

## Firmware version 18.99.3.p26.11 to 18.99.3.p27.6

**Wi-Fi**
- Fixed driver crash occurring during system suspend multiple test iterations in a cycle.
- Resolved an issue where the driver entered an infinite loop during WLAN firmware download, preventing the module from booting successfully.
- Fixed issue where the DUT remained stuck transmitting HE data frames using 802.11ax modulation and failed to fallback during long-range testing scenarios.
- Fixed scan timeout and device hang issue during active scanning operations.

**Bluetooth**
- No Command Complete event received from the controller for the LE Read Remote Transmit Power Level command
- LE Enhanced Read Transmit Power Level and LE Read Remote Transmit Power Level commands return status 0x11 (Unsupported Feature or Parameter Value).
- First eSCO link is delayed due to missing max_slot and max_slot_req transmissions from the controller over the air.
- No Disconnection Complete event received for eSCO from the controller, followed by the controller entering a hang state.
- No uplink audio is heard during the call.
- Unexpected power output observed during firmware reload and active scan disable operations.
Command credit is lost for the Create Connection command.
- Max Tx power value is not updated via the HCI_CMD_UPDATE_TX_MAX_PWR_LVL command during RF testing.
Crash occurs during execution of Bluetooth Classic TX/RX test.
- BLE link disconnects with a Connection Timeout error following the Data Length Update procedure.

**Parent page:** [IW611/IW612 SDIO-UART)](../topics/sd-uart_iw612.md)