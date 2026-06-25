[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 16.92.10.p233.2 to 16.92.21.p11.2

**Bluetooth**
- The ACL link with iPhone is disconnected due to error code “REMOTE DEVICE TERMINATED CONNECTION DUE TO LOW RESOURCES.

## Firmware version 16.92.21.p11.2 to 16.92.21.p41.1

**Wi-Fi**
- Fix Channel Occupancy Time \(COT\) for HT20/MCS0 within 6 msec.

## Firmware version 16.92.21.p41.1 to 16.92.21.p55.3

**Wi-Fi**
- Once DUT PAN profile gets disconnection with remote device, then DUT reconnection fails for successive connection trials.

**Bluetooth**
- DUT Bluetooth Classic and Bluetooth LE RX test mode fails to receive the packets and host is failing to derive the various parameters.

## Firmware version 16.92.21.p55.3 to 16.92.21.p76.3

**Bluetooth**
- DUT Bluetooth and Bluetooth LE TX test mode fails to set the power continuously and there is a difference between the configured and measured power.

## Firmware version 16.92.21.p76.3 to 16.92.21.p84.3

**Bluetooth**
- DUT HFP link gets disconnected with Remote phone, when it starts OPP file transfer to Remote device.

## Firmware version 16.92.21.p84.3 to 16.92.21.p84.128

None.

## Firmware version 16.92.21.p84.3 to 16.92.2.p119.11

**Wi-Fi**
- For the DRCS with P2P GO provisioning use-case, a Scan timeout is observed when STA is connected to EX-AP and Ex-Client is connected to GO.
- With DRCS enabled, when STA is connected to the EX-AP P2P data pause/stuck is observed.
- When connecting STA to Ex-AP in AP provisioning case, due to association status mismatch failures are observed in STA connection.

## Firmware version 16.92.2.p119.11 to 16.92.21.p137.4

**Wi-Fi**
- In RF test mode, TX power values are not updating after configured values in continuous wave transmit mode.

## Firmware version 16.92.2.p137.4 to 16.92.21.p142.3

**Wi-Fi**
- In RF test mode, Firmware command timeout is seen when switching between the enable and disable RF test modes.

## Firmware version 16.92.21.p142.3 to 16.92.21.p149.2

**Wi-Fi**
- Incorrect wake-up reason is observed during the validation of the wake on Wi-Fi feature.
- The firmware host command 0x112 and IOCTL failures are observed in dmesg when starting AP mode.

## Firmware version 16.92.21.p149.2 to 16.92.21.p149.4

**Wi-Fi**
- Added handling to read interrupt status register based on the device sleep state machine.
- Roaming test enhancements have been implemented through the blocking of scans during the authentication process.
- During suspend/resume stress testing, the DUT in station \(STA\) mode encountered a connection issue following the completion of the wakeup process.
- In DUT STA mode, the Reassociation response from the External Access Point was dropped after the acknowledgement was sent, which resulted in another Reassociation request, ultimately leading to Deauthentication from the External Access Point.
- Implemented changes to handle received frames with retry bit set from AP having same sequence number in the roaming scenario
- STAUT switches to 5GHz channel after ECSA from AP even if 5GHz channels are disabled on STAUT.

**Bluetooth**
- When the device connects with some headsets, some audio distortion is observed on downlink.

## Firmware version 16.92.21.p149.4 to 16.92.21.p151.4

**Wi-Fi**
- In the roaming test, the DUT failed to connect back to the previous AP due to association failure.
- In DFS test, Radar detection probability is found less than 60% in HT20/40MHz.
- Failure to set the MAC address with an init config file.

## Firmware version 16.92.21.p151.4 to 16.92.21.p151.5

**Wi-Fi**
- Wi-Fi reconnection failures observed in 2.4GHz frequency during long-run stress test scenarios.
- Fixed random kernel crash while performing stress test of automatic firmware recovery.

## Firmware version 16.92.21.p151.5 to 16.92.21.p153.4

**Wi-Fi**
- When ed-mac is enabled, probe responses are transmitted during interference signal.
- Wi-Fi fails to reconnect to 2.4 GHz Access Point over extended runtime.
- In the RF test mode, Issue with Wi-Fi Tx continuous wave mode on 2nd antenna was observed.
- Fixed an issue where Wi-Fi failed to reconnect to 2.4GHz Access Points during long run test

**Bluetooth**
- GPIO[0] is enabled for host wake-up functionality.
- When using the Coded PHY with S=8 coding scheme for BLE TX tests, the duty cycle vary.


## Firmware version 16.92.21.p153.4 to 16.92.21.p153.5

**Wi-Fi**
- p2p_find stops immediately upon receiving a connection request from the peer device, but DUT fails to establish the connection.

## Firmware version 16.92.21.p153.5 to 16.92.21.p153.7

**coexistence**
- Resolved issue where simultaneous operation of uAP and BLE scan in coexistence scenarios led to Wi-Fi data stalls and reduced BLE advertising report frequency.

## Firmware version 16.92.21.p153.7 to 16.92.21.p155.1

**Wi-Fi**
- Fixed P2P_FIND stopping prematurely when receiving connection requests from peer devices
- Fixed enhanced scan timeout occurring during extended idle periods in STA mode
- Corrected uAP deauth reason code to comply with IEEE standards for STA age-out scenarios
- Resolved connection failure in noisy conditions when antenna diversity is enabled with single antenna connected

**Bluetooth**
•	Fixed BLE duty cycle variation (16-65%) when using coded PHY with S=8 coding scheme in TX test mode

**Coex**
- Resolved Wi-Fi data stall and reduced BLE advertising reports in uAP+BLE scan coexistence scenarios

## Firmware version 16.92.21.p155.1 to 16.92.21.p160

**Wi-Fi**
- Fixed STA unable to connect to AP in noisy conditions with antenna diversity enabled.
- Fixed client devices unable to connect to DRCS AP.

**Bluetooth**
- Fixed BLE duty cycle variation (16-65%) when using coded PHY with S=8 coding scheme in TX test mode

**Coex**
- Resolved Wi-Fi data stall and reduced BLE advertising reports in uAP+BLE scan coexistence scenarios