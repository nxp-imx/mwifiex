[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 18.99.1.p154.40 to 18.99.2.p19.15

**Coexistence**
- Audio glitches observed on DUT as Master A2DP Source/Sink streaming with remote device when DUT Wi-Fi station is connected with external AP on 2.4 GHz.

## Firmware version 18.99.2.p19.15 to 18.99.2.p66.10

**Wi-Fi**
- Wake-up card timeout is observe when DUT AP changes the channels during TWT execution.
- DUT-STA does not stop sending the periodic null frames after executing TWT Teardown.

## Firmware version 18.18.99.2.p66.10 to 18.99.2.p66.17

**Wi-Fi**
- DUT-AP keeps sending RTS to client device which is turned off till age-out timer expires.
- Firmware fatal automatic recovery failed in long run stress testing.
- DUT wakeup interval found unexpected for successive wakeups in TWT session of specific Service period which can be more than 10mins.
- DUT station stuck observed after sending the deuathication due to unspecified reason in a disconnected state.
- Scan commda timeout is seen when performed scan while doing Auto-Tx in HE 80MHz mode

**Bluetooth**
- When A2DP steaming is initiated during an ongoing HFP call, A2DP link lose observed due to LMP response timeout \(Frequency of occurrence 4/5 times\)
- Link Stability in presence of multiple Bluetooth links under optimization
- In long run with Bluetooth Scatternet along with eSCO link established scenario, random DUT hang is observed.

**Coexistence**
- A2DP Audio Glitches are observed in the presence of Open Thread UDP Tx Traffic, when DUT is configured as Open Thread Leader/Router.

## Firmware version 18.99.2.p66.17 to 18.99.3.p10.1

**Wi-Fi**
- STAUT does not follow the configured wake-up duration.|
|Wi-Fi, Bluetooth/802.15.4 OpenThread \(OT\) coexistence \(IW612 only\)|
- High OT ping loss\(&gt;90%\) observed in the presence of Wi-Fi traffic and A2DP streaming.
- High OT-UDP-RX throughput drop observed in the presence of A2DP streaming on high antenna isolation.
- Low COEX throughput values observed in dual-radio and tri-radio cases with 20 dbm OT-TX power.

## Firmware version 18.99.3.p10.1 to 18.99.3.p15.8

**Wi-Fi**
- In DUT STA mode, a scan command timeout was observed during WPA3-FT PSK RSSI based roaming test on long run.
- In roaming test using wpa\_cli between multiple APs, the firmware is unstable when the RSSI of the AP is very low.

**Bluetooth**
- Authentication failure observed for ACL link, in presence of LE link.
- In dual HFP configuration, background noise heard on first audio link.

**LE Audio**
- In stress testing of 2-CIS, collision of ISO packet &amp; ATT data affects ISO anchor point scheduling.
- DUT is generating BIG sync lost event randomly after some inactivity.
- Second CIS establishment always fails when creating 2 CIS over one ACL with interleaved packing.
- Sometimes the connection timeout for CIS establishment event is observed on second/third CIS link while creating all 4/2 CIS together.

**Zigbee**
Zigbee firmware crash observed, after six devices joined to network.

**Wi-Fi, Bluetooth and 802.15.4 \(Open Thread\) coexistence**
- Randomly audio glitch observed, in the presence of Wi-Fi + Open thread UDP-TX traffic.
- Wi-Fi throughput goes 60% down when DUT working as slave role is connected to mobile phone.|

## Firmware version 18.99.3.p15.8 to 18.99.3.p15.13

**Wi-Fi**
- In the DRCS test, the firmware scan command timeout is observed when the DUT-STA tries to connect with an external AP using the wrong password, and a mobile tries to associate with the DUT AP.
- In the DRCS test, connection failures are seen when a mobile tries to associate with the DUT AP, and the DUT-STA tries to connect with an external AP using the wrong password.
- During TX power and regulatory test, kernel warning observed when `tx-power` values are not the same for 20 MHz, 40 MHz, and 80 MHz bonded channels.|
|Bluetooth|- Randomly, the DUT hang has been observed while connected with the peer device on Bluetooth/Bluetooth LE link for a long duration.

**Wi-Fi, Bluetooth and 802.15.4 \(Open Thread\) coexistence**
- In an LNT network of 50 nodes running for more than 24 hours, a segmentation fault error and app crash can occur.
- In an LNT Network of 10 nodes, when high traffic is running on each node, ZC and ZR got terminated because of MAC split errors.

## Firmware version 18.99.3.p15.13 to 18.99.3.p21.15

**Wi-Fi**
- In the DRCS test, DUT AP sent fewer beacons resulting ext. STA disconnections when DUT-STA already performing scan operation.

**Wi-Fi, Bluetooth and 802.15.4 \(Open Thread\) coexistence**
- Audio cuts observed when running the DUT STA coexistence RVR test.
- Firmware dump observed when the DUT STA is connected with an external AP and tries to connect a Bluetooth device.

## Firmware version 18.99.3.p21.15 to 18.99.3.p23.6

**Wi-Fi**
- In RF test mode, the TX power readback value is not expected in HE 80 MHz MCS11 data rate.
- In Easymesh test, `sys_cfg_channel` command error observed during ethernet onboarding with third party AP.
- In Easymesh test, `parse_1905` packet error observed during ethernet onboarding with third party AP.

**OpenThread \(OT\)**
- When working with OpenThread SPI, bus sharing is not supported with other SPI devices.

**Wi-Fi, Bluetooth and 802.15.4 \(Open Thread\) coexistence**
- High OT Ping loss is observed in the presence of Wi-Fi traffic and A2DP streaming in the closed environment. OT-UDP traffic cannot initiate in the presence of A2DP+Wi-Fi traffic.
- Wi-Fi throughput degraded by 35% when LE audio stream 1-BIS is running in the background.
- The DUT fails to start the mobile hotspot when Bluetooth and OT traffic is ongoing.

## Firmware version 18.99.3.p23.6 to 18.99.3.p23.20

**Wi-Fi**
- Firmware auto recovery failures seen during long run stress test in DUT-AP mode.
- Wi-Fi firmware automatic recovery failures are seen during stress test in DUT-AP mode running Iperf traffic.
- In Firmware automatic recovery test, firmware download fail observed on long run while DUT operating in AP mode.
- In long run Easymesh stress testing, error “Out of memory” is observed on Agent3.
- In Easymesh testing, a 20% ping loss observed in RX from Controller to a random Agent.
- In Easymesh, Topology Notification Messages are not sent properly when clients are getting connected and disconnected to MAUT \(MultiAP agent under test\).
- During suspend/resume stress test, "Wakeup card timeout\(0\)!" was observed.
- Enhancement: TSMC/UMC dual foundry support per FPCN 202405001F01 including golden calibration support for parts from TSMC/UMC foundries.

**Wi-Fi and Bluetooth coexistence**
- Wi-Fi command timeout is seen when there is A2DP streaming from phone.

## Firmware version 18.99.3.p23.20 to 18.99.3.p25.7

**Wi-Fi**
- Low throughput is observed when EDMAC is enabled with JP country code and DUT connected to NEC Aterm WG1200HP2 AP.
- Kernel warning fixes for cfg80211\_calculate\_bitrate+0x210 invalid rate.
- In DUT STA mode, Incorrect RSSI is reported to the wpa\_supplicant when ext AP keeps changing RSSI values.
- The scan interval does not transition back to the long interval even after the RSSI exceeds the configured threshold.
- Firmware hang issue observed when STAUT tries to connect ex-AP with the wrong password.

**Bluetooth**
- When 251 bytes of data set as extended advertisement data, over the air data length is set as 242 bytes.

## Firmware version 18.99.3.p25.7 to 18.99.3.p25.10

**Wi-Fi**
- In the DUT AP mode, DHCP timeout observed on the external STA when switching security from one enterprise to another enterprise.
- In automatic firmware recovery, random kernal crash observed during stress test.
- One of the pattern for ETSI Type 3 ETSI pulses has detection rate of 0% during the CAC period on specific setup

**Bluetooth**
- The firmware fails to assign connection handles during BIG (Broadcast Isochronous Group) creation.

## Firmware version 18.99.3.p25.10 to 18.99.3.p26.1

**Wi-Fi**
- Added CSI - Ambient Motion Index (AMI) feature support.

## Firmware version 18.99.3.p26.1 to 18.99.3.p26.7

**Wi-Fi**
- Fixed an issue causing incorrect output for the HCI "Read Transmit Power Level" and "Read Enhanced Transmit Power Level" commands.
- The DUT continues to transmit HE data frames with 802.11ax modulation and prevents it from falling back to more robust legacy OFDM rates. As a result, the data path fails silently, despite control connectivity.
- Addressed an inconsistency in background scan reporting where RSSI values were occasionally misreported as above threshold despite actual signal strength being lower
- Fixed issue where bgscan continued using short interval despite RSSI recovery, observed specifically with NetworkManager (nmcli) on STAUT.
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

**Wi-Fi and 802.15.4 and Zigbee (Open Thread) Coex**
- Segmentation fault observed on Zigbee Coordinator (ZC) during device joining in LNT with parallel method using ZB_CLI on Linux.

## Firmware version 18.99.3.p26.10 to 18.99.3.p26.11

**Wi-Fi**
- False secondary GI detection and throughput drop observed when duplicate RTS feature is enabled.
- uAP and STA fail to perform ranging in associated mode under FTM.
- Added validations for bandwidth configurations to avoid user errors in FTM Request.

**Bluetooth**
- Fixed issue where ISCAN/PSCAN re-enable was ineffective due to immediate disable after connection.

**Zigbee**
- Integrated DSR fixes in ZB_CLI for Zigbee Coordinator on Linux.

## Firmware version 18.99.3.p26.11 to 18.99.3.p27.6

**Wi-Fi**
- Fixed driver crash occurring during system suspend multiple test iterations in a cycle.
- Resolved an issue where the driver entered an infinite loop during WLAN firmware download, preventing the module from booting successfully.
- Fixed issue where the DUT remained stuck transmitting HE data frames using 802.11ax modulation and failed to fallback during long-range testing scenarios.
- Fixed scan timeout and device hang issue during active scanning operations.
- Failure of NAN discovery when initiated from a third-party reference device (Phone)

**Bluetooth**
- First LE link experiences impact during connect-disconnect activity on the second LE link.
- Connection error occurs when attempting to hand over the connection to a different radio interface.
- Two simultaneous LE connections established with the same remote device.
- Connection failure occurs due to disallowed Authentication Request command
- DUT responds to HCI_Sniff_Subrating with an undefined HCI status code.
- DUT does not respond to the HCI_Switch_Role command with an HCI_Role_Change event.
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