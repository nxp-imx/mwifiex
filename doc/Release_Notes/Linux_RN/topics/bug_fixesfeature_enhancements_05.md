[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 16.92.21.p76.2 to 16.92.21.p76.5

None.

## Firmware version 16.92.p99.2 to 16.92.2. p119.3

**Wi-Fi**
- During connect/disconnect stress testing, a 4-way Handshake Timeout is observed due to which the Ex-STA cannot connect to the AP.

## Firmware version 16.92.2.p119.3 to 16.92.21.p137.4

**Wi-Fi**
- In RF test mode, TX power values are not updating after configured values in continuous wave transmit mode.

## Firmware version 16.92.21p137.4 to 16.92.21.p142.3

**Wi-Fi**
- In RF test mode, TX power values are not updating after configured values in continuous wave transmit mode.
- In RF test mode, firmware command timeout is seen when switching between the enable and disable RF test modes.

## Firmware version 16.92.21.p142.3 to 16.92.21.p149.2

**Wi-Fi**
- In DUT-AP Mode, the firmware command timeout is observed when sending a large file via SCP along with continuous ping operation.

## Firmware version 16.92.21.p149.2 to 16.92.21.p149.4

**Wi-Fi**
- Roaming test enhancements have been implemented through the blocking of scans during the authentication process.
- During suspend/resume stress testing, the DUT in station \(STA\) mode encountered a connection issue following the completion of the wakeup process.
- In DUT STA mode, the Reassociation response from the External Access Point was dropped after the acknowledgement was sent, which resulted in another Reassociation request, ultimately leading to Deauthentication from the External Access Point.
- Implemented changes to handle received frames with retry bit set from AP having same sequence number in the roaming scenario.

**Bluetooth**
- When the device connects with some headsets, some audio distortion is observed on downlink.

## Firmware version 16.92.21.p149.4 to 16.92.21.p151.4

**Wi-Fi**
- In the roaming test, the DUT failed to connect back to the previous AP due to association failure.
- In P2P mode, Connection failure is observed when the device is in the listen state.
- Fail to set MAC address with an init config file.


## Firmware version 16.92.21.p151.4 to 16.92.21.p151.5

**Wi-Fi**
- Fixed random kernel crash while performing stress test of automatic firmware recovery.

## Firmware version 16.92.21.p151.5 to 16.92.21.p153.4

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

**Coex**
- Fixed command timeouts occurring during Wi-Fi/BT on/off stress testing
- Fixed a connectivity issue in which the DUT failed to establish further connections after extended execution of the LMP_au_rand flooding attack test
