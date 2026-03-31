[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 18.99.5.p61 to 18.25.5.p65

**Wi-Fi**
- Fixed random kernel crash while performing stress test of automatic firmware recovery.

## Firmware version 18.99.5.p65 to 18.25.5.p70

**Wi-Fi**
- Added CSI - Ambient Motion Index (AMI) feature support.

## Firmware version 18.99.25.5.p70 to 18.25.5.p73

**Wi-Fi**
- CMD timeouts observed during stress testing in concurrent uAP + STA mode.

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

**Bluetooth**
- BLE connection failure observed with a peer device running the nRF Connect application.

## Firmware version 18.99.5.p83 to 18.99.5.p86

**Wi-Fi**
- Fixed nested deadlock occurring in cfg80211_netdev_notifier_call() during system suspend.
- Improved driver lock handling within cfg80211_inform_bss() to prevent potential deadlocks.

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
