[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 16.92.10.p218 to 16.92.10.p219.3

**Wi-Fi**
- Added support for 40 MHz band in 2.4 GHz BGN mode for AP and STA

## Firmware version 16.92.10.p219.3 to 16.92.10.p219.5

None.

## Firmware version 16.92.10.p219.5 to 16.92.21.p41

None.

## Firmware version 16.92.21.p41 to 16.92.21.p55.3

**Wi-Fi**
- P2P-client fails to re-connect to DUT-P2P-GO mode after internal-STA connects to external-AP on different channel.
- Internal-STA disconnects from external-AP shortly after starting DUT-P2P-GO mode.
- DUT in STA only mode fails to connect with specific hotspot.

## Firmware version 16.92.p55.3 to 16.92.21.p76.2

**Bluetooth**
- DUT SPP link gets disconnected with Remote, when DUT creates A2DP SINK profile connection with Remote device.

## Firmware version 16.92.21.p76.2 to 16.92.21.p84.4

**Bluetooth**
- DUT HFP link gets disconnected with Remote phone, when it starts OPP file transfer to Remote device.

**Coexistence**
- DUT A2DP sink audio glitches observed when it starts Wi-Fi data traffic with Station device on BGN 20 MHz.
- DUT is not able to connect with Bluetooth device and not able to sustain LE connection, when it starts receiving the Wi-Fi data traffic with Station/Access Point on BGN 20 MHz.

## Firmware version 16.92.21.p84.4 to 16.92.21.p119.3

None.

## Firmware version 16.92.21.p119.3 to 16.92.21.p137.4

**Wi-Fi**
- In RF test mode, Firmware command timeout seen when stopping the on-going transmit via tx continuous mode.
- In RF test mode, Firmware command timeout is seen when switching between the enable and disable RF test modes.

**Bluetooth**
- Sometimes when the DUT is streaming A2DP data to another remote device, the DUT role switch request is failing to the first remote device.

## Firmware version 16.92.21.p137.4 to 16.92.21.p149.2

**Wi-Fi**
- During the roaming test with Meraki APs, the DUT STA explicitly sends a deauthentication frame to the previous AP to clean the STA list cache of the AP.<br/>
- In RF test mode, the transmission does not happen with tx\_frame for 5 GHz VHT 20 MHZ/40 MHZ MCS9 2SS and VHT80 MCS0 and MCS9 1SS and 2SS data rates if the 2.4 GHz test started before the 5 GHz VHT test.
- In DUT STA mode, the SCHED\_SCAN\_RESULTS are not updated to wpa\_supplicant layer during suspend and resume stress testing.

**Coexistence**
- Wakeup card timeout issue observed when executing `iwconfig` command during Bluetooth inquiry.

## Firmware version 16.92.21.p149.2 to 16.92.21.p149.4

**Wi-Fi**
- Roaming test enhancements have been implemented through the blocking of scans during the authentication process.
- During suspend/resume stress testing, the DUT in station \(STA\) mode encountered a connection issue following the completion of the wakeup process.
- In DUT STA mode, the Reassociation response from the External Access Point was dropped after the acknowledgement was sent, which resulted in another Reassociation request, ultimately leading to Deauthentication from the External Access Point.
- Implemented changes to handle received frames with retry bit set from AP having same sequence number in the roaming scenario.

**Bluetooth**
- When the device connects with some headsets, audio distortion is observed on downlink.

## Firmware version 16.92.21.p149.4 to 16.92.21.p151.4

**Wi-Fi**
- In the roaming test, the DUT failed to connect back to the previous AP due to association failure.
- Fails to set the MAC address with an *init config* file.

## Firmware version 16.92.21.p151.4 to 16.92.21.p151.5

**Wi-Fi**
- Fixed random kernel crash while performing stress test of automatic firmware recovery.

## Firmware version 16.92.21.p151.5 to 16.92.21.p153.4

**Wi-Fi**
- Wi-Fi driver reload fails if the fw_name parameter is removed from wifi_mod_para.conf.

## Firmware version 16.92.21.p153.4 to 16.92.21.p153.5

**Wi-Fi**
- p2p_find stops immediately upon receiving a connection request from the peer device, but DUT fails to establish the connection.

## Firmware version 16.92.21.p153.5 to 16.92.21.p153.7

**Wi-Fi**
- Resolved issue where Remain-on-Channel was not triggered during authentication after fallback to AP1.
- Fixed failure in configuring WLAN to "1x1 SMPS RBC" mode when Bluetooth is enabled.
- Resolved Wi-Fi crash with wake up card timeout error after initiating the A2DP Bluetooth profile streaming.
- Fixed issue where antenna configuration changed unexpectedly when connecting DUT STA to an AP on a DFS channel.
- Addressed 802_11_LINK_STATS command timeout during roaming in the 5 GHz band with WPA3 security.
- Resolved data block issue observed during roaming in the 5 GHz band