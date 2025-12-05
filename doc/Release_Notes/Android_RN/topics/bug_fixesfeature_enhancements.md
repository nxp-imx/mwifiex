[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 16.92.10.p213.4 to 16.92.21.p26.1

None.

## Firmware version 16.92.21.p26.1 to 16.92.21.p55.3

None.

## Firmware version 16.92.21.p55.3 to 16.92.21.p76.2

Bluetooth

- Random mute pattern observed during A2DP\_SRC streaming with Ref1 when OPP\_TX started with Ref2
- Unknown Connection Identifier observed during connection with 2nd LE HID in presence of 1st LE-HID.

## Firmware version 16.92.21.p76.2 to 16.92.21.p84.4

Wi-Fi

- Low throughput is observed in VHT80 mode for RX with all security modes.

## Firmware version 16.92.21.p84.4 to 16.92.21.p119.3

Bluetooth

- After disconnecting LE link, sometime disconnect complete event is delayed by 30 seconds. So, the next re-connection is possible only after 30 seconds.

## Firmware version 16.92.21.p119.3 to 16.92.21.p137.3

None.

## Firmware version 16.92.21.p137.3 to 16.92.21.p137.4

None.

## Firmware version 16.92.21.p137.4 to 16.92.21.p149.2

None.

## Firmware version 16.92.21.p149.2 to 16.92.21.p151.4

Wi-Fi

- In the roaming test, the DUT fails to connect back to the previous AP due to association failure.
- Failure to set the MAC address with an init config file.

## Firmware version 16.92.21.p151.4 to 16.92.21.p151.5

Wi-Fi
- Fixed random kernel crash while performing stress test of automatic firmware recovery.

## Firmware version 16.92.21.p151.5 to 16.92.21.p153.4

Wi-Fi
- Wi-Fi driver reload fails if the fw_name parameter is removed from wifi_mod_para.conf

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

**Parent page:** [88W8997-PCIe-UART)](../topics/pcie-uart_88w8997.md)