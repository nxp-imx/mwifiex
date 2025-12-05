[Link to index page](../index.md)

# Bug fixes/feature enhancements {#topic_5600d04f-a93f-418b-8029-a9f4c8244260}

## Firmware version 16.92.10.p218 to 16.92.10.p219.3 {#section1 .section}

Wi-Fi

- Added support for 40 MHz band in 2.4 GHz BGN mode for AP and STA

## Firmware version 16.92.10.p219.3 to 16.92.21.p219.5 {#section2 .section}

None.

## Firmware version 16.92.10.p219.5 to 16.92.21.p41 {#section3 .section}

None.

## Firmware version 16.92.21.p41 to 16.92.21.p55.3 {#section4 .section}

Wi-Fi

- Added support for 40 MHz band in 2.4 GHz BGN mode for AP and STA
- P2P-client fails to re-connect to DUT-P2P-GO mode after internal-STA connects to external-AP on different channel.
- Internal-STA disconnects from external-AP shortly after starting DUT-P2P-GO mode.
- DUT in STA only mode fails to connect with specific hotspot.

## Firmware version 16.92.21.p55.3 to 16.92.21.p76.2 {#section5 .section}

Bluetooth

- DUT SPP link gets disconnected with Remote, when DUT creates A2DP SINK profile connection with Remote device.

## Firmware version 16.92.21.p76.2 to 16.92.21.p84.4 {#section6 .section}

Bluetooth

- DUT HFP link gets disconnected with Remote phone, when it starts OPP file transfer to Remote device.

Coexistence

- DUT A2DP sink audio glitches observed when it starts Wi-Fi data traffic with Station device on BGN 20MHz.
- DUT is not able to connect with Bluetooth device and not able to sustain LE connection, when it starts receiving the Wi-Fi data traffic with Station/Access Point on BGN 20MHz.

## Firmware version 16.92.21.p84.4 to 16.92.21.p119.3 {#section7 .section}

Bluetooth

- After disconnecting LE link, sometime disconnect complete event is delayed by 30 seconds. So the next re-connection is possible only after 30 seconds.

## Firmware version 16.92.21.p119.3 to 16.92.21.p137.3 {#section8 .section}

None.

## Firmware version 16.92.21.p137.3 to 16.92.21.p137.4 {#section9 .section}

None.

## Firmware version 16.92.21.p137.4 to 16.92.21.p149.2 {#section10 .section}

None.

## Firmware version 16.92.21.p149.2 to 16.92.21.p151.4 {#section11 .section}

Wi-Fi

- In the roaming test, the DUT fails to connect back to the previous AP due to association failure.
- Failure to set the MAC address with an init config file.

## Firmware version 16.92.21.p151.4 to 16.92.21.p153.4

**Wi-Fi**
- Wi-Fi driver reload fails if the fw_name parameter is removed from wifi_mod_para.conf.

## Firmware version 16.92.21.p153.4 to 16.92.21.p153.5

**Wi-Fi**
- p2p_find stops immediately upon receiving a connection request from the peer device, but DUT fails to establish the connection.

## Firmware version 16.92.21.p153.5 to 16.92.21.p153.7


**Wi-Fi**
- Resolved issue where Remain-on-Channel was not triggered during authentication after fallback to AP1.
- Fixed issue where antenna configuration changed unexpectedly when connecting DUT STA to an AP on a DFS channel.
- Addressed 802_11_LINK_STATS command timeout during roaming in the 5 GHz band with WPA3 security.
- Resolved data block issue observed during roaming in the 5 GHz band

**Coex**
- Fixed failure in configuring WLAN to "1x1 SMPS RBC" mode when Bluetooth is enabled.
- Resolved Wi-Fi crash with wake up card timeout error after initiating the A2DP Bluetooth profile streaming.

**Parent page:** [SD-UART 88W8997](../topics/sd-uart_88w8997.md)

