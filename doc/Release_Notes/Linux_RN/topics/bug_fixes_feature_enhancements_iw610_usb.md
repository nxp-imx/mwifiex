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

## Firmware version 18.99.25.5.p76 to 18.25.5.p83

**Wi-Fi**
- Fixed firmware command timeout 0xb3 observed during deauthentication stress testing
- Fixed issue where beacon transmission became stuck and could not be recovered by the TX watchdog mechanism
- Integrated ZBOSS fix to support concurrent processing of multiple Permit Joining requests

**Bluetooth**
- BLE connection failure observed with a peer device running the nRF Connect application.