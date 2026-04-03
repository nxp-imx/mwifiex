[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 16.92.10.p208 to 16.92.21.p11.1

Bluetooth

- Fixed the disconnect complete event getting delayed by 30 seconds, so next re-connection was possible only after 30 seconds.

## Firmware version 16.92.10.p11.1 to 16.92.21.p41.3

None

## Firmware version 16.92.10.p41.3 to 16.92.21.p41.4

Bluetooth

- DUT as peripheral and DUT as central starts connection for LE link simultaneously, if link with DUT as peripheral gets connected before link with DUT as central then link with DUT as central gets disconnected.

## Firmware version 16.92.21.p41.4 to 16.92.21.p69.3

Bluetooth

- DUT pairing with LE HoGP remote device fails with authentication failure error.
- When DUT connected for HFP call and perform stress test for Bluetooth link connect disconnect then DUT firmware becomes unresponsive

Coexistence

- Sometimes in dual A2DP mode, glitches are observed and Wi-Fi RX throughput drops.

## Firmware version 16.92.21.p69.3 to 16.92.21.p76.2

Bluetooth

- When DUT A2DP streaming is ongoing and another LE device is connected with DUT and DUT is performing LE scan makes DUT firmware in bad condition for stress test.
- DUT is connected for OPP profile with remote device and when transfer file to remote device then Bluetooth link gets disconnected.

Coexistence

- LE peripheral pairing gets failed with Mobile device when Wi-Fi is enabled on a single antenna device.

## Firmware version 16.92.21.p76.2 to 16.92.21.p76.5

None

## Firmware version 16.92.21.p76.5 to 16.92.21.p99.2

Bluetooth

- When Bluetooth A2DP streaming is ongoing with first remote device then DUT failed for encryption with another LE remote device.

## Firmware version 16.92.21.p99.2 to 16.92.21.p119.3

Bluetooth

- After disconnecting LE link, sometime disconnect complete event is delayed by 30 seconds. So the next re-connection is possible only after 30 seconds.

## Firmware version 16.92.21.p119.3 to 16.92.21.p137.3

Bluetooth

- During parallel firmware load, Bluetooth firmware fails to load if Wi-Fi firmware is loaded first, the firmware load sequence should be Bluetooth firmware followed by Wi-Fi firmware.

## Firmware version 16.92.21.p137.3 to 16.92.21.p142.3

None.

## Firmware version 16.92.21.p142.3 to 16.92.21.p149.2

None

## Firmware version 16.92.21.p149.2 to 16.92.21.p151.4

Wi-Fi

- In P2P mode, connection failure is observed when the device is in listen state.

## Firmware version 16.92.21.p151.4 to 16.92.21.p153.4

**Wi-Fi**
- 1-second data transfer interruption observed between AP and STA, causing communication breakage.

## Firmware version 16.92.21.p153.4 to 16.92.21.p153.5

**Wi-Fi**
- p2p_find stops immediately upon receiving a connection request from the peer device, but DUT fails to establish the connection.

## Firmware version 16.92.21.p153.5 to 16.92.21.p153.7

**Wi-Fi**
- STAUT reported association failure with status code 1 and reason code 0 when an incorrect password was used for WPA3 Ex-AP.

## Firmware version 16.92.21.p153.7 to 16.92.21.p155.1
**Wi-Fi**
- Resolved 0xa4 command timeout occurring during long-term STA connection to specific access points
- Fixed 802_11_LINK_STATS command timeout during 5 GHz roaming with WPA3 security
- Resolved data transmission blocking issue during 5 GHz band roaming

**Coexistance**
- Fixed command timeouts occurring during Wi-Fi/BT on/off stress testing
- Fixed a connectivity issue in which the DUT failed to establish further connections after extended execution of the LMP_au_rand flooding attack test


**Parent file:** [SD-UART 88W8987](../topics/sd-uart_88w8987.md)

