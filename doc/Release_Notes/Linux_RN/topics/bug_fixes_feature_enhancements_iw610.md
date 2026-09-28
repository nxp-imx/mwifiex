[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 18.99.5.p43 to 18.99.5.p51

**Wi-Fi**
- Firmware wakeup card timeout observed during stress testing of firmware independent reset in DUT STA connected state.

**Bluetooth LE**
- While connecting 10 BLE devices with connection interval of 30-50ms connections are not stable.

## Firmware version 18.99.5.p51 to 18.99.5.p56

**Wi-Fi**
- In TX power feature, the readback of the RU TX Power command output is not anticipated for negative set TX-power values.<br/>
- Firmware command timeout during roaming between two APs in stress test.

**Bluetooth LE**
- Baud rate mismatch seen with the calibration data when using the read calibration data command.

## Firmware version 18.99.5.p56 to 18.25.5.p61

**Wi-Fi**
- Observed high OT-Ping loss and 0kbps throughput on OT traffic when the uAP traffic is started.<br/>
- Parallel independent reset \(IR\) \(Wi-Fi and NB IR issued independently from host\) can cause system stuck in stress testing. Robust solution implementation in process.

## Firmware version 18.99.5.p61 to 18.25.5.p65

**Wi-Fi**
- During the driver load and unload stress test, the firmware fails to load sometime.
- Fixed random kernel crash while performing stress test of automatic firmware recovery.

## Firmware version 18.99.5.p65 to 18.25.5.p70

**Wi-Fi**
- Added CSI - Ambient Motion Index (AMI) feature support.

## Firmware version 18.99.25.5.p70 to 18.25.5.p73

**Wi-Fi**
- CMD timeouts observed during stress testing in concurrent uAP + STA mode.
- Unable to set the BA timeout using the addbapara mlanutl command for the mlan0 interface

**Zigbee**
- Unable to set the tx_power using the Zigbee CLI command.

## Firmware version 18.99.25.5.p73 to 18.25.5.p76

**Wi-Fi**
- Wi-Fi firmware hang observed during AP + STAUT concurrent operation scenario.

**Wi-Fi and 15.4**
- Low OT throughput observed on IW610 when Wi-Fi is connected (idle) and Thread is active

## Firmware version 18.25.5.p76 to 18.99.5.p83

**Wi-Fi**
- Fixed firmware command timeout 0xb3 observed during deauthentication stress testing
- Fixed issue where beacon transmission became stuck and could not be recovered by the TX watchdog mechanism
- Integrated ZBOSS fix to support concurrent processing of multiple Permit Joining requests
- Unable to set the BA timeout using the addbapara mlanutl command for the mlan0 interface.

**Bluetooth**
- BLE connection failure observed with a peer device running the nRF Connect application.

## Firmware version 18.99.5.p83 to 18.99.5.p86

**Wi-Fi**
- Fixed nested deadlock occurring in cfg80211_netdev_notifier_call() during system suspend.
- Improved driver lock handling within cfg80211_inform_bss() to prevent potential deadlocks

**Zigbee**
- Improved Zigbee LNT stability

## Firmware version 18.99.5.p86 to 18.99.8.p3

**Wi-Fi**
- Added support for loading different firmware images for IW610 variants on the same platform.
- Resolved multiple unnecessary channel switches when Automatic Group Channel Selection (AGCS) channel load threshold is triggered.
- Fixed beacon transmission stuck issue and enhanced TX watchdog recovery mechanism
- Resolved P2P connection failure when operating as Group Client (GC) with certain smartphone models

**Bluetooth**
- Fixed BLE connection rejection error "Limited Resources" when establishing second connection with 251-byte DLE configured on first connection
- Added support for configuring TX power for LE Coded PHY mode
- Improved Bluetooth stability and reliability when using SDIO interface

## Firmware version 18.99.8.p3 to 18.99.8.p16

**Wi-Fi**
- Updated configuration file to correctly set tx/rx antenna fields and remove redundant RF band parameter when radio_mode is specified.
- Fixed CSI (Channel State Information) capture failure when sniffer/monitor mode is enabled in unassociated state.
- Fixed external STA connection failure to DUT AP during concurrent connect-disconnect and flood ping tests.
- Resolved WiFi command timeout (0xa4) during Wi-Fi (uAP+STA) and Thread coexistence testing
- Fixed command timeout (0x23f) in STA+uAP DRCS mode during external AP channel switching

**Zigbee**
- Added GPIO naming auto-detection support in zb_mux.sh script for Zigbee multiplexer initialization

## Firmware version 18.99.8.p16 to 18.99.8.p52

**Wi-Fi**
- Fixed HMAC stuck during concurrent uAP + STA (DRCS) + OpenThread coexistence test.
- Fixed increased power consumption after Wi-Fi driver unload.
- Fixed self-managed regulatory table not being generated in latest firmware release.
- Fixed Wi-Fi IOCTL CMD 0x256 response error.

**Bluetooth**
- Added error code response in BT/BLE MFG firmware for invalid power configuration.

**OpenThread**
- Fixed OpenThread daemon crash in Wi-Fi + OT coexistence test

**Coex (Wi-Fi & OpenThread)**
- Fixed Wi-Fi command timeout during Wi-Fi (STA + uAP) + OpenThread coexistence test.

## Firmware version 18.99.8.p52 to 18.99.8.p136

**Wi-Fi**
- Fixed unexpected STA disconnect in monitor mode when other client disconnects from AP.
- Added support for 11az station-to-station ranging mode and fixed the mlancsi application hanging after initialization.
- Fixed Wi-Fi RF Test Mode continuous-wave tone transmission stopping during long runs.
- Fixed 802.11mc FTM ranging failures with QCA-based Aruba and Google access points; ACK now forces MCS0 to resolve the interop issue.
- Fixed a duplicate QoS Control field in host-injected QoS data frames; injected frames now carry a single, correct QoS Control field.
- Fixed a command timeout that occurred during Thread and Wi-Fi coexistence; concurrent Thread plus Wi-Fi operation now runs without command timeouts.
- Added ability to configure VHT capabilities on the P2P interface.
- Fixed incorrect Tx power readout via mlanutl txpwr.
- Fixed missing deauth on P2P client removal.
- Fixed incorrect behavior during sleep with Wake-on-WLAN.
- Fixed the "Card is removed" issue during USB suspend; the driver now detaches network interfaces before suspend and restores them if suspend fails.
- Removed the unsupported 2x2 stream control feature for USB-IW610, aligning driver capabilities with the hardware and preventing incorrect antenna-stream configuration on that interface.
- Fixed an SSID heap overflow in Multi-BSSID descriptor parsing; the driver now rejects SSIDs longer than 32 bytes and uses the correct destination buffer size.
- Added missing information-element bounds check.
- Fixed IE length underflow causing out-of-bounds read.
- Added element-length capping to close OOB patterns.
- Fixed a truncated length check in non-transmitted BSSID profile parsing; removing an 8-bit cast ensures oversized information elements are correctly rejected instead of accepted.
- Fixed an out-of-bounds write in non-transmitted extended supported rates handling; the driver now passes the correct remaining buffer size to the copy operation.
- Fixed STA association failure under stress testing.
- Fixed 0xA4 timeout after long idle period.
- Fixed a possible kernel stack overflow in OBSS coexistence event handling; the driver now validates the overlapping-BSS information-element length before copying it into the stack buffer.
- Fixed incorrect power-table values in mlanutl cfpinfo

**Bluetooth/LE**
- Fixed two LE connections to different devices sharing the same connection handle; each LE connection now receives its own unique handle.

**ZIgbee**
- Added a workaround for an assert in zdo_secur.c during network join, preventing the crash and allowing devices to join successfully.
- Added BRF support to enable or disable 15.4 channel-26 transmit power clamping
