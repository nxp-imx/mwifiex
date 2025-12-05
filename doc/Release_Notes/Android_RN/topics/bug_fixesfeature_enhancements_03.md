[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 16.92.10.p208 to 16.92.21.p11.1

Wi-Fi

- Fix for channel occupancy time \(COT\) for HT20/MCS0 within 6 mseconds

## Firmware version 16.92.21.p41.1 to 16.92.21.p55.3

Wi-Fi

- Once DUT PAN profile gets disconnection with remote device, then DUT reconnection fails for successive connection trials.
- DUT Bluetooth Classic &amp; Bluetooth LE RX test mode fails to receive the packets and host is failing to derive the various parameters.

## Firmware version 16.92.21.p55.3 to 16.92.21.p76.3

Bluetooth

- DUT Bluetooth &amp; Bluetooth LE TX test mode fails to set the power continuously and there is a difference between configured and measured power.

## Firmware version 16.92.21.p76.3 to 16.92.21.p84.3

Bluetooth

- The ACL link with iPhone is disconnected due to error code “REMOTE DEVICE TERMINATED CONNECTION DUE TO LOW RESOURCES”
- Random Bluetooth security link loss in concurrent Bluetooth classic and Bluetooth LE modes with AES
- DUT HFP link gets disconnected with Remote phone, when it starts OPP file transfer to Remote device.

## Firmware version 16.92.21.p84.3 to 16.92.21.p84.128

None.

## Firmware version 16.92.21.p84.128 to 16.92.21.p119.3

Bluetooth

- After disconnecting LE link, sometime disconnect complete event is delayed by 30 seconds. So the next re-connection is possible only after 30 seconds.

## Firmware version 16.92.21.p119.3 16.92.21.p119.11

Wi-Fi

- For the DRCS with P2P GO provisioning use-case, a scan timeout is observed when STA is connected to EX-AP and Ex-Client is connected to GO.
- With DRCS enabled, when STA is connected to the EX-AP P2P, data pause/stuck is observed. When connecting STA to Ex-AP in AP provisioning case, due to association status mismatch, failures are observed in STA connection.

## Firmware version 16.92.21.p119.11 16.92.21.p137.3

None.

## Firmware version 16.92.21.p137.3 16.92.21.p142.3

None.

## Firmware version 16.92.21.p142.3 16.92.21.p149.2

None.

## Firmware version 16.92.21.p149.2 16.92.21.p151.4

Wi-Fi

- When ed-mac is enabled, probe responses are transmitted during interference signal.
- In the roaming test, the DUT fails to connect back to the previous AP due to association failure.
- Failure to set the MAC address with an init config file.

## Firmware version 16.92.21.p151.4 to 16.92.21.p153.4

**Wi-Fi**
- When ed-mac is enabled, probe responses are transmitted during interference signal.
- Wi-Fi fails to reconnect to 2.4 GHz Access Point over extended runtime.
- In the RF test mode, Issue with Wi-Fi Tx continuous wave mode on 2nd antenna was observed.
- Fixed an issue where Wi-Fi failed to reconnect to 2.4GHz Access Points during long run test

**Bluetooth**
- When using the Coded PHY with S=8 coding scheme for BLE TX tests, the duty cycle vary.

## Firmware version 16.92.21.p153.4 to 16.92.21.p153.5

**Wi-Fi**
- p2p_find stops immediately upon receiving a connection request from the peer device, but DUT fails to establish the connection.

## Firmware version 16.92.21.p153.5 to 16.92.21.p153.7

**coexistence**
- Resolved issue where simultaneous operation of uAP and BLE scan in coexistence scenarios led to Wi-Fi data stalls and reduced BLE advertising report frequency.

**Parent page:** [SD-UART IW416](../topics/sd-uart_iw416.md)

